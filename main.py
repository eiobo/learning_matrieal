# /home/cwq/code/LLM_Server/main.py
import asyncio
from AiCore.AiInstanceRepo import AiInstanceRepository
from Utils.colorful_log import get_colorful_logger
from Utils import utils
from Utils.ai_factory import AIFactory
from protocol.websocket_server import WebSocketServer

async def background_task():
    """一个简单的后台任务，它会持续打印信息，以证明主程序没有被阻塞。"""
    bg_logger = get_colorful_logger()
    bg_logger.info("background_task starts, i will wait AI response and print messages! ")
    for i in range(100):
        bg_logger.info(f"background_task is running... {i+1}/100")
        await asyncio.sleep(0.1)
    bg_logger.info("background_task ends!")

def factory_test():
    # --------------------------------------- Factory test ---------------------------------------
    # load config
    cfg = utils.Util.get_config()
    factory = AIFactory(logger=get_colorful_logger(), config=cfg)

    # asr test
    # asr = factory.create_asr_instance()
    # output_text, success = asr.transcribe("Intermediate/tts_output/tts_output.wav")
    # if success:
    #     get_colorful_logger().info(f"ASR Output: {output_text}")

    # # OpusEncoder test
    # encoder = factory.create_encoder_instance()
    # if encoder._audio_to_opus_sync("Intermediate/tts_output/tts_output.wav", "test_output.wav"):
    #     get_colorful_logger().info("OpusEncoder test passed.")

    # # OpusDecoder test
    # decoder = factory.create_decoder_instance()
    # if decoder._decode_to_wav_sync("Intermediate/opus_decoder_input/opus_decoder_input.opus", "test_output.wav"):
    #     get_colorful_logger().info("OpusDecoder test passed.")

    # llm test
    # llm = factory.create_llm_instance()
    # response = llm.chat_sync("你好，今天的天气怎么样？")
    # print(response)

    # tts test
    async def ttstest():
        tts = factory.create_tts_instance()
        if await tts.text_to_speech_async("你好，今天的天气怎么样？", "aaaa"):
            get_colorful_logger().info("TTS test passed.")
    asyncio.run(ttstest())
    # -------------------------------------------------------------------------------------------


# test with : asyncio.run(llm2tts2opus())
async def llm2tts2opus():
    cfg = utils.Util.get_config()
    logger = get_colorful_logger(enable_file=True)
    ai_instance_repo = AiInstanceRepository(logger=logger, config=cfg)
    response = ai_instance_repo.llm.chat_sync("你好，今天的天气怎么样？")
    success = ai_instance_repo.encoder._audio_to_opus_sync(await ai_instance_repo.tts.text_to_speech_async(response, "bbb"))
    if success:
        logger.info("Audio encoding to Opus format succeeded.")

if __name__ == "__main__":
    # utils.Util.init_output_dirs({})
    # asyncio.run(main())
    cfg = utils.Util.get_config()
    logger = get_colorful_logger(enable_file=True)
    ws = WebSocketServer(logger=logger, config=cfg, ai_instance=AiInstanceRepository(logger=logger, config=cfg))
    try:
        asyncio.run(ws.start())
    except KeyboardInterrupt:
        logger.info("WebSocket server stopped.")

