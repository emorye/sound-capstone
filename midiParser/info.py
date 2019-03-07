import mido
import serial
import threading

mid = mido.MidiFile('midi/marioOverworld.mid')
# mid = mido.MidiFile('midi/marioGustyGarden.mid')

for i, track in enumerate(mid.tracks):
    print('Track {}: {}'.format(i, track.name))
    for msg in track:
      # if not msg.type == "note_on" and not msg.type == "note_off":
      if i==1:
        # print("  " + msg.type)
        print("  " + str(msg))