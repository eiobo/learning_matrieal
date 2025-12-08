// 极简占位：模拟 Opus 播放器接口，便于对接服务器端二进制帧
// 之后可替换为真正的解码器（如 wasm 版 libopus + WebAudio）
(function(global){
  class OpusPlayer {
    constructor(){
      this.reset();
      this.audioCtx = null;
    }
    reset(){
      this.frames = 0;
      this.started = false;
    }
    start(meta){
      this.started = true;
      this.frames = 0;
      if(!this.audioCtx){
        try{ this.audioCtx = new (window.AudioContext||window.webkitAudioContext)(); }catch(e){}
      }
      // 这里可根据 meta.sample_rate/frame_duration 做播放器预热
      console.log('[OpusPlayer] start', meta);
    }
    pushPacket(u8){
      // 这里应把单个 Opus 包送给解码器；当前仅统计帧数
      this.frames++;
      // console.debug('[OpusPlayer] packet', u8.length);
    }
    async finish(meta){
      console.log('[OpusPlayer] finish, total frames=', this.frames, meta);
      // 占位：播放一个极短的提示音，表示收到了音频数据
      if(this.audioCtx){
        const ctx=this.audioCtx; const dur=0.05; const osc=ctx.createOscillator(); const gain=ctx.createGain();
        osc.type='sine'; osc.frequency.value=660; gain.gain.setValueAtTime(0.05, ctx.currentTime);
        osc.connect(gain).connect(ctx.destination); osc.start(); osc.stop(ctx.currentTime + dur);
      }
      this.reset();
    }
  }
  global.OpusPlayer = OpusPlayer;
  // 在页面中实例化一个默认播放器
  window.addEventListener('load', ()=>{ if(!global.opusPlayer){ global.opusPlayer = new OpusPlayer(); } });
})(window);
