#!/usr/bin/env python3
"""The download page's pictures from the trailer (game pictures: they go only in build/site, never in git):
  build/site/media/screen.mp4  the cabinet's screen: the trailer's fights, 640x360, 30 fps, no sound track
  build/site/shots/00.webp     its first picture (the video's poster while it loads)
  build/site/media/poster.jpg  and media/og.jpg (the link preview): Pai and Sarah in the subway
Needs ffmpeg.

  tools/site/media.py TRAILER.mp4 [--from 0 --to 45 --still 12.6]
"""
import argparse, os, subprocess

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.path.join(ROOT, 'build', 'site')


def ff(*args):
    subprocess.run(['ffmpeg', '-v', 'error', '-y', *args], check=True)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('trailer')
    ap.add_argument('--from', dest='t0', type=float, default=0)    # seconds into the trailer: the fights,
    ap.add_argument('--to', dest='t1', type=float, default=45)     # before the end card
    ap.add_argument('--still', type=float, default=12.6)
    a = ap.parse_args()
    os.makedirs(os.path.join(OUT, 'media'), exist_ok=True)
    os.makedirs(os.path.join(OUT, 'shots'), exist_ok=True)
    ff('-ss', str(a.t0), '-to', str(a.t1), '-i', a.trailer, '-an', '-vf', 'scale=640:360:flags=lanczos,fps=30',
       '-c:v', 'libx264', '-preset', 'slow', '-crf', '24', '-pix_fmt', 'yuv420p', '-movflags', '+faststart',
       os.path.join(OUT, 'media', 'screen.mp4'))
    ff('-ss', str(a.t0), '-i', a.trailer, '-frames:v', '1', '-vf', 'scale=640:360:flags=lanczos',
       '-c:v', 'libwebp', '-quality', '86', os.path.join(OUT, 'shots', '00.webp'))
    for name in ('poster.jpg', 'og.jpg'):
        ff('-ss', str(a.still), '-i', a.trailer, '-frames:v', '1', '-vf', 'scale=1280:720:flags=lanczos',
           '-q:v', '3', os.path.join(OUT, 'media', name))
    print('made', os.path.join(OUT, 'media'), 'and', os.path.join(OUT, 'shots', '00.webp'))


if __name__ == '__main__':
    main()
