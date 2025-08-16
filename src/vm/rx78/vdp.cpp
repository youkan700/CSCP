/*
	BANDAI RX-78 Emulator 'eRX-78'

	Author : Takeda.Toshiya
	Date   : 2006.08.21 -

	[ vdp ]
*/

#include "vdp.h"

void VDP::initialize()
{
	memset(reg, 0, sizeof(reg));
	cmask = pmask = 0xff;
	bg = 0;
	create_pal();
	create_bg();
	
	// register event to interrupt
	register_vline_event(this);
}

void VDP::write_io8(uint32_t addr, uint32_t data)
{
	switch(addr & 0xff) {
	case 0xf5:
		reg[0] = data;
		create_pal();
		break;
	case 0xf6:
		reg[1] = data;
		create_pal();
		break;
	case 0xf7:
		reg[2] = data;
		create_pal();
		break;
	case 0xf8:
		reg[3] = data;
		create_pal();
		break;
	case 0xf9:
		reg[4] = data;
		create_pal();
		break;
	case 0xfa:
		reg[5] = data;
		create_pal();
		break;
	case 0xfb:
#if 0 //TEST//
		cmask = ((data & 1) ? 4 : 0) | ((data & 2) ? 1 : 0) | ((data & 4) ? 2 : 0); 
		cmask |= cmask << 4;
#else
		cmask = data;
#endif
		create_pal();
		break;
	case 0xfc:
		bg = data;
		create_bg();
		break;
	case 0xfe:
		pmask = data;
		break;
	}
}

void VDP::event_vline(int v, int clock)
{
	// vsync interrupt (not pending ???)
	if(v == 184) {
		d_cpu->set_intr_line(true, false, 0);
	}
}

void VDP::draw_screen()
{
#if 0 //TEST//
	// create screen
	for(int y = 0, src = 0; y < 184; y++) {
		uint8_t* dest0 = screen0[y];
		uint8_t* dest1 = screen1[y];
		
		for(int x = 0; x < 192; x += 8) {
			uint8_t p0 = (pmask & 0x01) ? vram[0][src] : 0;
			uint8_t p1 = (pmask & 0x02) ? vram[1][src] : 0;
			uint8_t p2 = (pmask & 0x04) ? vram[2][src] : 0;
			uint8_t p3 = (pmask & 0x08) ? vram[3][src] : 0;
			uint8_t p4 = (pmask & 0x10) ? vram[4][src] : 0;
			uint8_t p5 = (pmask & 0x20) ? vram[5][src] : 0;
			src++;
			
			dest0[x + 7] = ((p0 & 0x80) >> 7) | ((p1 & 0x80) >> 6) | ((p2 & 0x80) >> 5);
			dest0[x + 6] = ((p0 & 0x40) >> 6) | ((p1 & 0x40) >> 5) | ((p2 & 0x40) >> 4);
			dest0[x + 5] = ((p0 & 0x20) >> 5) | ((p1 & 0x20) >> 4) | ((p2 & 0x20) >> 3);
			dest0[x + 4] = ((p0 & 0x10) >> 4) | ((p1 & 0x10) >> 3) | ((p2 & 0x10) >> 2);
			dest0[x + 3] = ((p0 & 0x08) >> 3) | ((p1 & 0x08) >> 2) | ((p2 & 0x08) >> 1);
			dest0[x + 2] = ((p0 & 0x04) >> 2) | ((p1 & 0x04) >> 1) | ((p2 & 0x04) >> 0);
			dest0[x + 1] = ((p0 & 0x02) >> 1) | ((p1 & 0x02) >> 0) | ((p2 & 0x02) << 1);
			dest0[x + 0] = ((p0 & 0x01) >> 0) | ((p1 & 0x01) << 1) | ((p2 & 0x01) << 2);
			
			dest1[x + 7] = ((p3 & 0x80) >> 7) | ((p4 & 0x80) >> 6) | ((p5 & 0x80) >> 5);
			dest1[x + 6] = ((p3 & 0x40) >> 6) | ((p4 & 0x40) >> 5) | ((p5 & 0x40) >> 4);
			dest1[x + 5] = ((p3 & 0x20) >> 5) | ((p4 & 0x20) >> 4) | ((p5 & 0x20) >> 3);
			dest1[x + 4] = ((p3 & 0x10) >> 4) | ((p4 & 0x10) >> 3) | ((p5 & 0x10) >> 2);
			dest1[x + 3] = ((p3 & 0x08) >> 3) | ((p4 & 0x08) >> 2) | ((p5 & 0x08) >> 1);
			dest1[x + 2] = ((p3 & 0x04) >> 2) | ((p4 & 0x04) >> 1) | ((p5 & 0x04) >> 0);
			dest1[x + 1] = ((p3 & 0x02) >> 1) | ((p4 & 0x02) >> 0) | ((p5 & 0x02) << 1);
			dest1[x + 0] = ((p3 & 0x01) >> 0) | ((p4 & 0x01) << 1) | ((p5 & 0x01) << 2);
		}
	}
	
	// copy to screen buffer
	for(int y = 0; y < 184; y++) {
		scrntype_t* dest = emu->get_screen_buffer(y);
		uint8_t* src0 = screen0[y];
		uint8_t* src1 = screen1[y];
		
		for(int x = 0; x < 192; x++) {
			dest[x] = palette_pc[src0[x] ? src0[x] : src1[x] ? (src1[x] + 8) : 16];
		}
	}
#else
	// create screen
	for(int y = 0, src = 0; y < 184; y++) {
		uint8_t* dest0 = screen0[y];
		
		for(int x = 0; x < 192; x += 8) {
			uint8_t p0 = (pmask & 0x01) ? vram[0][src] : 0;
			uint8_t p1 = (pmask & 0x02) ? vram[1][src] : 0;
			uint8_t p2 = (pmask & 0x04) ? vram[2][src] : 0;
			uint8_t p3 = (pmask & 0x08) ? vram[3][src] : 0;
			uint8_t p4 = (pmask & 0x10) ? vram[4][src] : 0;
			uint8_t p5 = (pmask & 0x20) ? vram[5][src] : 0;
			src++;
			
			dest0[x + 7] = ((p0 & 0x80) >> 7) | ((p1 & 0x80) >> 6) | ((p2 & 0x80) >> 5) | ((p3 & 0x80) >> 4) | ((p4 & 0x80) >> 3) | ((p5 & 0x80) >> 2);
			dest0[x + 6] = ((p0 & 0x40) >> 6) | ((p1 & 0x40) >> 5) | ((p2 & 0x40) >> 4) | ((p3 & 0x40) >> 3) | ((p4 & 0x40) >> 2) | ((p5 & 0x40) >> 1);
			dest0[x + 5] = ((p0 & 0x20) >> 5) | ((p1 & 0x20) >> 4) | ((p2 & 0x20) >> 3) | ((p3 & 0x20) >> 2) | ((p4 & 0x20) >> 1) | ((p5 & 0x20) >> 0);
			dest0[x + 4] = ((p0 & 0x10) >> 4) | ((p1 & 0x10) >> 3) | ((p2 & 0x10) >> 2) | ((p3 & 0x10) >> 1) | ((p4 & 0x10) >> 0) | ((p5 & 0x10) << 1);
			dest0[x + 3] = ((p0 & 0x08) >> 3) | ((p1 & 0x08) >> 2) | ((p2 & 0x08) >> 1) | ((p3 & 0x08) >> 0) | ((p4 & 0x08) << 1) | ((p5 & 0x08) << 2);
			dest0[x + 2] = ((p0 & 0x04) >> 2) | ((p1 & 0x04) >> 1) | ((p2 & 0x04) >> 0) | ((p3 & 0x04) << 1) | ((p4 & 0x04) << 2) | ((p5 & 0x04) << 3);
			dest0[x + 1] = ((p0 & 0x02) >> 1) | ((p1 & 0x02) >> 0) | ((p2 & 0x02) << 1) | ((p3 & 0x02) << 2) | ((p4 & 0x02) << 3) | ((p5 & 0x02) << 4);
			dest0[x + 0] = ((p0 & 0x01) >> 0) | ((p1 & 0x01) << 1) | ((p2 & 0x01) << 2) | ((p3 & 0x01) << 3) | ((p4 & 0x01) << 4) | ((p5 & 0x01) << 5);
		}
	}
	
	// copy to screen buffer
	for(int y = 0; y < 184; y++) {
		scrntype_t* dest = emu->get_screen_buffer(y);
		uint8_t* src0 = screen0[y];
		
		for(int x = 0; x < 192; x++) {
			dest[x] = palette_pc[src0[x]];
		}
	}
#endif
}

void VDP::create_pal()
{
#if 0 //TEST//
	// create palette
	for(int i = 0; i < 8; i++) {
		uint8_t tmp = ((i & 1) ? reg[0] : 0) | ((i & 2) ? reg[1] : 0) | ((i & 4) ? reg[2] : 0);
		if(tmp & cmask) {
			tmp &= cmask;
		}
		uint16_t r = ((tmp & 0x11) == 0x11) ? 255 : ((tmp & 0x11) == 0x1) ? 127 : 0;
		uint16_t g = ((tmp & 0x22) == 0x22) ? 255 : ((tmp & 0x22) == 0x2) ? 127 : 0;
		uint16_t b = ((tmp & 0x44) == 0x44) ? 255 : ((tmp & 0x44) == 0x4) ? 127 : 0;
		
		palette_pc[i] = RGB_COLOR(r, g, b);
	}
	for(int i = 8; i < 16; i++) {
		uint8_t tmp = ((i & 1) ? reg[3] : 0) | ((i & 2) ? reg[4] : 0) | ((i & 4) ? reg[5] : 0);
		if(tmp & cmask) {
			tmp &= cmask;
		}
		uint16_t r = ((tmp & 0x11) == 0x11) ? 255 : ((tmp & 0x11) == 0x1) ? 127 : 0;
		uint16_t g = ((tmp & 0x22) == 0x22) ? 255 : ((tmp & 0x22) == 0x2) ? 127 : 0;
		uint16_t b = ((tmp & 0x44) == 0x44) ? 255 : ((tmp & 0x44) == 0x4) ? 127 : 0;
		
		palette_pc[i] = RGB_COLOR(r, g, b);
	}
#else
	// create palette
	for(int i = 1; i < 64; i++) {
		uint8_t tmp;
		uint8_t tmp0;
		uint8_t tmp1;
		uint8_t bm;
		tmp0 = tmp1 = 0;
		bm = 0x01;
		for (int j=0; j<6; j++) {
		    if (i & bm) {
			if (cmask & bm) tmp1 |= reg[j];
			else		tmp0 |= reg[j];
		    }
		    bm <<= 1;
		}
		tmp = (tmp1 != 0) ? tmp1 : tmp0;

		uint16_t r = ((tmp & 0x11) == 0x11) ? 255 : ((tmp & 0x11) == 0x1) ? 127 : 0;
		uint16_t g = ((tmp & 0x22) == 0x22) ? 255 : ((tmp & 0x22) == 0x2) ? 127 : 0;
		uint16_t b = ((tmp & 0x44) == 0x44) ? 255 : ((tmp & 0x44) == 0x4) ? 127 : 0;

		palette_pc[i] = RGB_COLOR(r, g, b);
	}
#endif
}

void VDP::create_bg()
{
	// create bg palette
	uint16_t r = ((bg & 0x11) == 0x11) ? 255 : ((bg & 0x11) == 0x1) ? 127 : 0;
	uint16_t g = ((bg & 0x22) == 0x22) ? 255 : ((bg & 0x22) == 0x2) ? 127 : 0;
	uint16_t b = ((bg & 0x44) == 0x44) ? 255 : ((bg & 0x44) == 0x4) ? 127 : 0;
	
#if 0 //TEST//
	palette_pc[16] = RGB_COLOR(r, g, b);
#else
	palette_pc[0] = RGB_COLOR(r, g, b);
#endif
}

#define STATE_VERSION	1

bool VDP::process_state(FILEIO* state_fio, bool loading)
{
	if(!state_fio->StateCheckUint32(STATE_VERSION)) {
		return false;
	}
	if(!state_fio->StateCheckInt32(this_device_id)) {
		return false;
	}
	state_fio->StateArray(palette_pc, sizeof(palette_pc), 1);
	state_fio->StateArray(reg, sizeof(reg), 1);
	state_fio->StateValue(bg);
	state_fio->StateValue(cmask);
	state_fio->StateValue(pmask);
	return true;
}

