#ifndef VRAM_HANDLER_H
#define VRAM_HANDLER_H

class VRAMHandler {
	private:

	public:
		VRAMHandler();
		virtual ~VRAMHandler();

		virtual bool Update() = 0; //Return false to destroy

};

#endif
