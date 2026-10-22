// Actual SDL GPU / Metal shaders and target recreation. Synthetic data only.
#include "../../src/gpu/r3d_gpu.h"
#include "../../src/host/display_color.h"
#include <cassert>
#include <cstdio>
#include <cmath>
static uint32_t pixel(SDL_Renderer *r,int x,int y){
 auto *a=SDL_RenderReadPixels(r,nullptr);assert(a);
 auto *b=SDL_ConvertSurface(a,SDL_PIXELFORMAT_ARGB8888);assert(b);
 uint32_t v=*reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(b->pixels)+y*b->pitch+4*x);
 SDL_DestroySurface(a);SDL_DestroySurface(b);return v&0xffffff;
}
int main(){
 assert(SDL_Init(SDL_INIT_VIDEO));
 SDL_SetHint(SDL_HINT_GPU_DRIVER,"metal");
 auto *w=SDL_CreateWindow("ROM-free Model 3 GPU check",320,240,SDL_WINDOW_HIDDEN);assert(w);
 auto *r=SDL_CreateRenderer(w,"gpu");assert(r);
 R3dGpu gpu;assert(gpu.init(SDL_GetGPURendererDevice(r)));
 auto *crt=colour_shader(SDL_GetGPURendererDevice(r));assert(crt);SDL_ReleaseGPUShader(SDL_GetGPURendererDevice(r),crt);
 video::Scene scene;video::Raster prep;std::vector<uint16_t> tex(2048*2048);
 std::vector<m3::Model3::TexUpload> uploads;
 for(auto size:{std::pair{128,96},std::pair{2868,1320},std::pair{128,96}}){
  prep.resize(size.first,size.second);prep.prepare(scene);
  assert(gpu.output(size.first,size.second));assert(gpu.render(scene,prep,tex,uploads));
  std::vector<uint32_t> pixels;assert(gpu.read_output(pixels));
  assert(pixels.size()==size_t(size.first)*size.second);
  for(auto v:pixels)assert(v==0);
 }
 auto *t=SDL_CreateTexture(r,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STATIC,4,4);assert(t);
 uint32_t pixels[16];for(auto &v:pixels)v=0xff404040;assert(SDL_UpdateTexture(t,nullptr,pixels,16));
 for(int gain:{100,150,200,100}){assert(prepare_overlay_view(r));assert(draw_game_picture(r,t,gain));unsigned c=64*gain/100;assert(pixel(r,160,120)==c*0x010101);}
 for(float aspect:{4.0f/3,16.0f/10,16.0f/9,64.0f/27}){
  assert(prepare_game_view(r,int(384*aspect),384));SDL_FRect area{};
  assert(SDL_GetRenderLogicalPresentationRect(r,&area));assert(std::abs(area.w/area.h-aspect)<0.004f);
 }
 SDL_DestroyTexture(t);gpu.shutdown();SDL_DestroyRenderer(r);SDL_DestroyWindow(w);SDL_Quit();
 puts("PASS: Model 3 Metal/CRT shaders, native target recreation, brightness and all fixed aspect presentations");
}
