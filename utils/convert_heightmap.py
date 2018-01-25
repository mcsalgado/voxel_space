#!/usr/bin/env python3

import sys
import os.path
from PIL import Image

# NOTE(mcsalgado): I can't believe I had to write this script orz

file_path = sys.argv[1]
file_name, _ = os.path.splitext(file_path)

with Image.open(file_path) as im:
    im.convert('L')
    im.save(f'{file_name}.bmp')
