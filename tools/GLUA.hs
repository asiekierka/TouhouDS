{-

  GLUA.hs

  Generates LUA binding boilerplate code from a file containing C function
  signatures. Specifically designed to generate a mock interface for C++
  objects. Having separate internal and external interfaces is good for
  maintainability :)
  
-}

module Main where

import System
import Data.Char
import Data.List
import Debug.Trace
import Text.ParserCombinators.Parsec
import Text.ParserCombinators.Parsec.Language (emptyDef, javaStyle)
import qualified Text.ParserCombinators.Parsec.Token as P
import Text.Printf
import System.FilePath

type Arg = (String, Type)

data Decl = DFuncHeader FuncHeader
          | DEnum CEnum          

data FuncHeader = FuncHeader {
    fhReturnType :: Type,
    fhName :: String,
    fhArgs :: [Arg]
}
  deriving (Read, Show)

data Type = TVoid
          | TFixed
          | TInt
          | TBool
          | TString
  deriving (Eq, Read, Show)

data CEnum = CEnum {
    cenumName :: String,
    cenumFields :: [CEnumField]
}

data CEnumField = CEnumField String (Maybe Int)
  
srcTypeMapping :: [(String, Type)]
srcTypeMapping = [
    ("void",   TVoid),
    ("fixed",  TFixed),
    ("int",    TInt),
    ("bool",   TBool),
    ("string", TString)]
          
dstTypeMapping :: [(Type, String)]
dstTypeMapping = [
    (TVoid,   "void"),
    (TFixed,  "s32"),
    (TInt,    "int"),
    (TBool,   "bool"),
    (TString, "const char*")]

luaStackTypeMapping :: [(Type, String)]
luaStackTypeMapping = [
    (TFixed,  "number"),
    (TInt,    "integer"),
    (TBool,   "boolean"),
    (TString, "string")]

filterFuncHeaders :: [Decl] -> [FuncHeader]
filterFuncHeaders decls = foldr f [] decls
  where f (DFuncHeader fh) list = fh : list
        f _ list = list

filterEnums :: [Decl] -> [CEnum]
filterEnums decls = foldr f [] decls
  where f (DEnum e) list = e : list
        f _ list = list


    
main :: IO ()
main =
 do args <- getArgs
    let glueFilePath = head args
    processFile glueFilePath
    
processFile :: FilePath -> IO()
processFile path =
    let f = takeFileName $ dropExtension path
        asObject = takeExtension f == ".obj"
    in  process path (dropExtension f) asObject
    
process :: FilePath -> String -> Bool -> IO() 
process glueFilePath objName asObject =
 do fileContents <- readFile glueFilePath
    case parse parser glueFilePath fileContents of
        Left err -> putStr "Error: " >> print err
        Right decls ->
             do let fhs = filterFuncHeaders decls
                let enums = filterEnums decls
                let (h, c1, c2) = genFile objName asObject fhs enums
                let folder = dropFileName glueFilePath
                writeFile (folder </> headerFileName objName) h
                writeFile (folder </> code1FileName objName) c1
                writeFile (folder </> code2FileName objName) c2
    return ()

    
nullAlt :: [a] -> b -> b -> b
nullAlt list nonNullAlt nullAlt = if null list then nullAlt else nonNullAlt
    
funcPrefix objName = nullAlt objName (objName++"_") ""
gluePrefix objName = "glue_" ++ funcPrefix objName
regFuncName objName = "glua_register" ++ nullAlt objName ('_':objName) ""
constrFunc objName = gluePrefix objName ++ "new"
destrFunc objName = gluePrefix objName ++ "gc"
tableName objName = toUpper (head objName) : tail objName
metaTableName objName = tableName objName ++ "Meta"
headerFileName s = s ++ ".glue.h"
code1FileName s = s ++ ".glue.cpp"
code2FileName s = s ++ ".stub.cpp"

genFile :: String -> Bool -> [FuncHeader] -> [CEnum] -> (String, String, String)
genFile objName asObject fhs enums = 
    let (luaHs, normalHs) = unzip (map (genHeaders objName asObject) fhs)
        (luaCs, normalCs) = unzip (map (genCode objName asObject) fhs)
        
        gluaConstrDecls = [genGluaFuncHeader (constrFunc objName), genGluaFuncHeader (destrFunc objName)]
        gluaHeaderDecls = if asObject then gluaConstrDecls ++ luaHs else luaHs
        headerFile = printf (codeTemplate_headerFile objName)
                        (unlines $ "":gluaHeaderDecls ++ ["", genEnumDecls enums])
                        (unlines $ "":normalHs)

        stubIncludes = "#include \"" ++ headerFileName objName++ "\""
        stubConstrDecls = [genConstrFunc objName, genDestrFunc objName]
        stubFile = unlines $ [stubIncludes, "", genRegFuncCode objName asObject fhs (genRegEnums enums)]
                    ++ (if asObject then stubConstrDecls else [""]) ++ normalCs
        
    in  (headerFile, unlines $ [stubIncludes, ""] ++ luaCs, stubFile)

genGluaFuncHeader :: String -> String
genGluaFuncHeader name = "int " ++ name ++ "(lua_State* L);"

genRegFuncHeader :: String -> String
genRegFuncHeader objName = "int " ++ regFuncName objName ++ "(lua_State* L);"
         
genRegFuncCode :: String -> Bool -> [FuncHeader] -> String -> String
genRegFuncCode objName asObject fhs regEnums = unlines $
    table funcRegTableName (defaultFuncs ++ if asObject then [] else  map toEntry fhs) ++
    table methodRegTableName (defaultMethods ++ if asObject then map toEntry fhs else []) ++
    [regFooter]
  where defaultFuncs   = if asObject then ["{\"new\", &" ++ constrFunc objName ++ "},"] else []
        defaultMethods = if asObject then ["{\"__gc\", &" ++ destrFunc objName ++ "},"] else []
        table s entries = ("static const struct luaL_reg " ++ s ++ "[] = {") :
            map ('\t':) (entries ++ ["{NULL, NULL}"]) ++ ["};"]
        funcRegTableName = "glua" ++ nullAlt objName ('_':objName) "" ++ "_f"
        methodRegTableName = "glua" ++ nullAlt objName ('_':objName) "" ++ "_m"
        toEntry (FuncHeader _ nm _) = "{\"" ++ nm ++ "\", &" ++ gluePrefix objName ++ nm ++ "},"
        regFooter = printf codeTemplate_reg (regFuncName objName) regEnums (metaTableName objName)    
                        methodRegTableName (tableName objName) funcRegTableName

genEnumDecls :: [CEnum] -> String
genEnumDecls enums = unlines $ map genEnumDecl enums

genEnumDecl :: CEnum -> String
genEnumDecl (CEnum name fields) =
    "typedef enum {\n\t" ++ concat (intersperse ",\n\t" $ map printEnumField fields) ++
        "\n} " ++ name ++ ";\n"

printEnumField :: CEnumField -> String
printEnumField (CEnumField name (Just i)) = name ++ " = " ++ show i
printEnumField (CEnumField name _) = name
   
genRegEnums :: [CEnum] -> String
genRegEnums enums = unlines $ map genRegEnum enums

genRegEnum :: CEnum -> String
genRegEnum (CEnum name fields) = unlines $ map ('\t':) $
    ["lua_newtable(L);"] ++
    map genField fields ++
    ["lua_setglobal(L, \"" ++ name ++ "\");"]
  where genField (CEnumField name _) = "glua_setEnumField(L, \"" ++ name ++ "\", " ++ name ++ ");"
    
genHeaders :: String -> Bool -> FuncHeader -> (String, String)
genHeaders objName asObject (FuncHeader rt name args) = (lua, normal)
  where normal = printf codeTemplate_stubHeader (genType rt) (funcPrefix objName ++ name) as ++ ";"
        as  = genStubArgs objName asObject args
        lua = genGluaFuncHeader $ gluePrefix objName ++ name

genStubArgs :: String -> Bool -> [Arg] -> String        
genStubArgs objName asObject fargs =
    let objArg = tableName objName ++ "* obj"
        as     = map genArg fargs
    in  concat $ intersperse ", " $ if asObject then objArg:as else as
        
genType :: Type -> String
genType t = case lookup t dstTypeMapping of
                Just x -> x
                _ -> "void"
       
genArg :: Arg -> String
genArg (name, ty) = genType ty ++ " " ++ name
    
genCode :: String -> Bool -> FuncHeader -> (String, String)
genCode objName asObject (FuncHeader rt name fargs) = (glue, stub)
  where glue = printf codeTemplate_glue (gluePrefix objName ++ name)
                    (if asObject then 1 + length fargs else length fargs)
                    (glueRet True rt) (glueRet False rt)
                    (case rt of
                        TVoid -> "\treturn 0;"
                        _     -> printf "\t%s;\n\treturn 1;" (luaPushArg rt "r")
                    )
                    
        glueRet :: Bool -> Type -> String
        glueRet safe TVoid = '\t' : stubCall safe
        glueRet safe _     = printf "\t%s r = %s" (genType rt) (stubCall safe)
                            
        stubCall :: Bool -> String
        stubCall safe = printf "%s%s(%s);" (funcPrefix objName) name
                            (concat $ intersperse ", " $ stubCallArgs safe)
        stubCallArgs s = if asObject
                            then luaPopObjArg s : luaPopArgs s 2 fargs
                            else luaPopArgs s 1 fargs
        
        --luaPopObjArg True = printf "(%s*)luaL_checkudata(L, 1, \"%s\")" (tableName objName) (tableName objName)
        luaPopObjArg _ = printf "(%s*)lua_touserdata(L, 1)" (tableName objName)

        luaPopArgs safe n (x:xs) = luaPopArg safe n x : luaPopArgs safe (n+1) xs
        luaPopArgs safe n _ = []
        luaPopArg :: Bool -> Int -> Arg -> String
        luaPopArg safe i (tn, ty) =
            case lookup ty luaStackTypeMapping of
                Just str -> if safe then case ty of
                                            TBool -> printf "lua_toboolean(L, %d)" i
                                            _ -> printf "luaL_check%s(L, %d)" str i
                                    else printf "lua_to%s(L, %d)" str i
                _ -> error $ "Illegal argument: " ++ tn
                
        luaPushArg ty var =
            case lookup ty luaStackTypeMapping of
                Just str -> "lua_push" ++ str ++ "(L, " ++ var ++ ")"
                _ -> error $ "Illegal argument: " ++ show ty
                
        stub = printf codeTemplate_stub (genType rt) (funcPrefix objName ++ name)
                            (genStubArgs objName asObject fargs)
    
genConstrFunc :: String -> String
genConstrFunc objName = 
    let tn = tableName objName
    in  printf codeTemplate_constr (constrFunc objName) tn tn tn tn (metaTableName objName)
    
genDestrFunc :: String -> String
genDestrFunc objName = 
    let tn = tableName objName
    in  printf codeTemplate_destr (destrFunc objName) tn tn tn

-------------------------------------------------------------------------------
--- PARSER --------------------------------------------------------------------
-------------------------------------------------------------------------------
    
--Lexer  
lexer :: P.TokenParser ()
lexer = P.makeTokenParser javaStyle

whiteSpace = P.whiteSpace lexer
identifier = P.identifier lexer
commaSep = P.commaSep lexer
parens = P.parens lexer
symbol = P.symbol lexer
natural = P.natural lexer
    
--Parser
parser :: Parser [Decl]
parser =
 do whiteSpace
    decls <- many (penum <|> pfunc)
    return decls

pfunc :: Parser Decl
pfunc =
 do t <- ptype        
    n <- identifier
    a <- parens $ commaSep parg
    symbol ";"
    return $ DFuncHeader (FuncHeader t n a)

parg :: Parser (String, Type)
parg =
 do t <- ptype
    n <- identifier
    return (n, t)
    
ptype :: Parser Type
ptype =
 do id <- identifier
    case lookup id srcTypeMapping of
        Just x -> return x
        _ -> fail $ "Illegal type: " ++ id

penum :: Parser Decl
penum =
 do symbol "enum"
    name <- identifier
    symbol "{"
    fields <- commaSep penumField
    symbol "}"
    symbol ";"
    return $ DEnum (CEnum name fields)

penumField :: Parser CEnumField
penumField = try
    (do name <- identifier
        symbol "="
        intval <- natural
        return (CEnumField name (Just $ fromIntegral intval)))
      <|>
    (do name <- identifier
        return (CEnumField name Nothing))

-------------------------------------------------------------------------------
--- TEMPLATES -----------------------------------------------------------------
-------------------------------------------------------------------------------

codeTemplate_headerFile objName = unlines [
    "#ifndef _GLUA_" ++ map toUpper objName ++ "_H_",
    "#define _GLUA_" ++ map toUpper objName ++ "_H_",
    "",
    "#ifdef __cplusplus",
    "extern \"C\" {",
    "#endif",
    "",
    "#include <stdio.h>",
    "#include \"lua.h\"",
    "#include \"lualib.h\"",
    "#include \"lauxlib.h\"",
    "",
    "#ifndef GLUA_ERR",
    "#define GLUA_ERR(x, y...) printf(x, ## y)",
    "#endif",
    "",
    "#ifndef GLUA_CHECK_NUM_ARGS",
    "#define GLUA_NUM_ARGS_ERROR_FMT \"LUA Error: Wrong number of args: expected %%d, got %%d.\\n\"",    
    "#define GLUA_CHECK_NUM_ARGS(L, num) \\",
    "\tif (lua_gettop(L) != num) { \\",
    "\t\tGLUA_ERR(GLUA_NUM_ARGS_ERROR_FMT, num, lua_gettop(L)); \\",
    "\t\treturn 0; \\",
    "\t}",
    "#endif",
    "%s",
    "#ifdef __cplusplus",
    "}",
    "#endif",
    "",
    "int " ++ regFuncName objName ++ "(lua_State* L);",
    "%s",
    "#endif"]

codeTemplate_glue :: String
codeTemplate_glue = unlines [
    "int %s(lua_State* L) {",
    "\tGLUA_CHECK_NUM_ARGS(L, %d);",
    "#ifdef DEBUG",
    "%s",
    "#else",
    "%s",
    "#endif",
    "%s",
    "}"]
    
codeTemplate_reg :: String
codeTemplate_reg = unlines [
    "static void glua_setEnumField(lua_State* L, const char* name, int value) {",
    "\tlua_pushstring(L, name);",
    "\tlua_pushinteger(L, value);",
    "\tlua_settable(L, -3);",
    "\tlua_pushinteger(L, value);",
    "\tlua_setglobal(L, name);",
    "}",
    "",
    "int %s(lua_State* L) {",
    "%s",
    "\tluaL_newmetatable(L, \"%s\");",
    "\tlua_pushstring(L, \"__index\");",
    "\tlua_pushvalue(L, -2);",
    "\tlua_settable(L, -3);",
    "\tluaL_openlib(L, NULL, %s, 0);",
    "\tluaL_openlib(L, \"%s\", %s, 0);",
    "\treturn 1;",
    "}"]
    
codeTemplate_stubHeader :: String
codeTemplate_stubHeader = "%s %s(%s)"

codeTemplate_stub :: String
codeTemplate_stub = unlines [
    codeTemplate_stubHeader ++ " {",
    "\t//TODO: Implement",
    "}"]
        
codeTemplate_constr = unlines [
    "int %s(lua_State* L) {",
    "\t%s* obj = (%s*)lua_newuserdata(L, sizeof(%s));",
    "\tobj = new(obj) %s();",
    "\tlua_getmetatable(L, \"%s\");",
    "\tlua_setmetatable(L, -2);",
    "\treturn 1;",
    "}"]

codeTemplate_destr = unlines [
    "int %s(lua_State* L) {",
    "\t%s* obj = (%s*)lua_touserdata(L, 1);",
    "\tobj->~%s();",
    "\treturn 0;",
    "}"]
        