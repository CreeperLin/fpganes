/***************************************************************************************************
** fpga_nes/hw/src/ppu/ppu.v
*
*  Copyright (c) 2012, Brian Bennett
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted
*  provided that the following conditions are met:
*
*  1. Redistributions of source code must retain the above copyright notice, this list of conditions
*     and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright notice, this list of
*     conditions and the following disclaimer in the documentation and/or other materials provided
*     with the distribution.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
*  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
*  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
*  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
*  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
*  WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*  Picture processing unit block.
***************************************************************************************************/

module ppu
(
  input  wire        clk_in,        // 100MHz system clock signal
  input  wire        rst_in,        // reset signal
  input  wire [ 2:0] ri_sel_in,     // register interface reg select
  input  wire        ri_ncs_in,     // register interface enable
  input  wire        ri_r_nw_in,    // register interface read/write select
  input  wire [ 7:0] ri_d_in,       // register interface data in
  input  wire [ 7:0] vram_d_in,     // video memory data bus (input)
  output wire        hsync_out,     // vga hsync signal
  output wire        vsync_out,     // vga vsync signal
  output wire [ 2:0] r_out,         // vga red signal
  output wire [ 2:0] g_out,         // vga green signal
  output wire [ 1:0] b_out,         // vga blue signal
  output wire [ 7:0] ri_d_out,      // register interface data out
  output wire        nvbl_out,      // /VBL (low during vertical blank)
  output wire [13:0] vram_a_out,    // video memory address bus
  output wire [ 7:0] vram_d_out,    // video memory data bus (output)
  output wire        vram_wr_out    // video memory read/write select
);

//
// PPU_VGA: VGA output block.
//
wire [5:0] vga_sys_palette_idx;
wire [9:0] vga_nes_x;
wire [9:0] vga_nes_y;
wire [9:0] vga_nes_y_next;
wire       vga_pix_pulse;
wire       vga_vblank;

ppu_vga ppu_vga_blk(
  .clk_in(clk_in),
  .rst_in(rst_in),
  .sys_palette_idx_in(vga_sys_palette_idx),
  .hsync_out(hsync_out),
  .vsync_out(vsync_out),
  .r_out(r_out),
  .g_out(g_out),
  .b_out(b_out),
  .nes_x_out(vga_nes_x),
  .nes_y_out(vga_nes_y),
  .nes_y_next_out(vga_nes_y_next),
  .pix_pulse_out(vga_pix_pulse),
  .vblank_out(vga_vblank)
);


endmodule

