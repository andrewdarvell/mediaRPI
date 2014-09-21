import pygst
pygst.require('0.10')
import gst
import time

class Player(object):
    def __init__(self, channel):
        self.pipeline = gst.Pipeline("RadioPipe")
        self.player = gst.element_factory_make("playbin", "player")
        pulse = gst.element_factory_make("pulsesink", "pulse")
        fakesink = gst.element_factory_make("fakesink", "fakesink")

        self.player.set_property('uri', channel)
        self.player.set_property("audio-sink", pulse)
        self.player.set_property("video-sink", fakesink)

    def play(self):
        self.pipeline.set_state(gst.STATE_PLAYING)

    def stop(self):
        self.pipeline.set_state(gst.STATE_PAUSED)


player = Player("http://http-live.sr.se/p1-mp3-192")


while 1:
    command = raw_input("command\n")
    if command == "play":
        player.play()
    if command == "stop":
        player.stop()
    if command == "new":
        channel = raw_input("new radio channel")
        player.stop()
        player = Player(channel)
        player.play()
    if command == "exit":
        break
