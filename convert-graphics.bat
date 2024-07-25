SET TEXCONV=java -jar TextureConverter.jar

cd tools

@rem menu images
%TEXCONV% -4 ..\src-art\menu\menu_bg.png ..\TouhouDS\img
%TEXCONV% -7 -nodither ..\src-art\menu\menu_tex.png ..\TouhouDS\img
%TEXCONV% -7 -nodither ..\src-art\menu\menu_title_tex.png ..\TouhouDS\img
%TEXCONV% -4 ..\src-art\menu\options_bg.png ..\TouhouDS\img
%TEXCONV% -4 ..\src-art\menu\title.png ..\TouhouDS\img
%TEXCONV% -4 ..\src-art\menu\cowhou-ds.png ..\TouhouDS\img
%TEXCONV% -6 ..\src-art\menu\default_game_banner.png ..\TouhouDS\img
%TEXCONV% -6 ..\src-art\menu\ds_buttons_16.png ..\TouhouDS\img
%TEXCONV% -4 ..\src-art\menu\results.png ..\TouhouDS\img

@rem in-game images
%TEXCONV% -7 -nodither ..\src-art\osd.png ..\TouhouDS\img\game

@rem textures
%TEXCONV% -1 ..\src-art\tex_player_fx.png ..\TouhouDS\img\game
%TEXCONV% -1 ..\src-art\tex_item.png ..\TouhouDS\img\game
%TEXCONV% -1 ..\src-art\tex_bullet.png ..\TouhouDS\img\game
%TEXCONV% -1 ..\src-art\tex_bullet2.png ..\TouhouDS\img\game
%TEXCONV% -1 ..\src-art\tex_enemy.png ..\TouhouDS\img\game
%TEXCONV% -4 ..\src-art\tex_background.png ..\TouhouDS\img\game
%TEXCONV% -6 ..\src-art\tex_explosion.png ..\TouhouDS\img\game
%TEXCONV% -1 ..\src-art\tex_boss.png ..\TouhouDS\img\game

@rem chara
%TEXCONV% -1 ..\src-art\chara\reimu\tex_reimu.png ..\TouhouDS\chara\reimu
%TEXCONV% -7 ..\src-art\chara\reimu\icon.png ..\TouhouDS\chara\reimu
%TEXCONV% -1 ..\src-art\chara\marisa\tex_marisa.png ..\TouhouDS\chara\marisa
%TEXCONV% -7 ..\src-art\chara\marisa\icon.png ..\TouhouDS\chara\marisa
%TEXCONV% -1 ..\src-art\chara\sakuya\tex_sakuya.png ..\TouhouDS\chara\sakuya
%TEXCONV% -7 ..\src-art\chara\sakuya\icon.png ..\TouhouDS\chara\sakuya
%TEXCONV% -1 ..\src-art\chara\orin\tex_orin.png ..\TouhouDS\chara\orin
%TEXCONV% -7 ..\src-art\chara\orin\icon.png ..\TouhouDS\chara\orin
%TEXCONV% -1 ..\src-art\chara\youmu\tex_youmu.png ..\TouhouDS\chara\youmu
%TEXCONV% -7 ..\src-art\chara\youmu\icon.png ..\TouhouDS\chara\youmu
%TEXCONV% -1 ..\src-art\chara\yuka\tex_yuka.png ..\TouhouDS\chara\yuka
%TEXCONV% -7 ..\src-art\chara\yuka\icon.png ..\TouhouDS\chara\yuka
%TEXCONV% -1 ..\src-art\chara\sanae\tex_sanae.png ..\TouhouDS\chara\sanae
%TEXCONV% -7 ..\src-art\chara\sanae\icon.png ..\TouhouDS\chara\sanae

%TEXCONV% -1 ..\src-art\chara\sa_marisa_b\tex_marisa.png ..\TouhouDS\chara\sa_marisa_b
%TEXCONV% -7 ..\src-art\chara\sa_marisa_b\icon.png ..\TouhouDS\chara\sa_marisa_b

@rem bombs
%TEXCONV% -6 ..\src-art\bomb\default\default.png ..\TouhouDS\bomb\default
%TEXCONV% -6 ..\src-art\bomb\bomb_reimu\bomb_reimu.png ..\TouhouDS\bomb\bomb_reimu
%TEXCONV% -1 ..\src-art\bomb\master_spark\master_spark.png ..\TouhouDS\bomb\master_spark
%TEXCONV% -1 ..\src-art\bomb\fangirl_youmu\fangirl_youmu.png ..\TouhouDS\bomb\fangirl_youmu
%TEXCONV% -1 ..\src-art\bomb\sakuya\sakuya.png ..\TouhouDS\bomb\sakuya
%TEXCONV% -6 ..\src-art\bomb\youmu\youmu.png ..\TouhouDS\bomb\youmu
%TEXCONV% -1 ..\src-art\bomb\yuka\yuka.png ..\TouhouDS\bomb\yuka
%TEXCONV% -1 ..\src-art\bomb\sanae\sanae.png ..\TouhouDS\bomb\sanae

@rem boss
%TEXCONV% -1 ..\src-art\boss\cirno\tex_boss_cirno.png
%TEXCONV% -1 ..\src-art\boss\chen\tex_boss_chen.png
%TEXCONV% -1 ..\src-art\boss\flandrisa\tex_boss_flandrisa.png
%TEXCONV% -1 ..\src-art\boss\tewi\tex_boss_tewi.png
%TEXCONV% -1 ..\src-art\boss\mokou\tex_boss_mokou.png
%TEXCONV% -1 ..\src-art\boss\yuyucow\tex_boss_yuyucow.png

cd ..
