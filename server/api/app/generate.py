from PIL import Image, ImageFont
import unicodedata
import datetime

def len_fullwidth(text):
    return -(-sum([(1, 2)[unicodedata.east_asian_width(t) in 'FWA'] for t in text])//2)

def getJSON(message):
  img = Image.new('L', (len_fullwidth(message)*8, 8), color=0)
  img_w, img_h = img.size
  font = ImageFont.truetype('/code/app/font/misaki_gothic.ttf', 8)
  mask = font.getmask(message)
  mask_w, mask_h = img.size
  d = Image.core.draw(img.im, 0)
  d.draw_bitmap(((img_w - mask_w)/2, (img_h - mask_h)/2), mask, 255)
  buff = [0]*mask_w
  for i in range(mask_h):
      for j in range(mask_w):
          if(img.getpixel((j,i)) > 85):
            img.putpixel((j,i), 255)
            buff[j] += 2**i
          else:
            img.putpixel((j,i), 0)
  # img.show()
  return {"mat_len": mask_w + 4, "data": buff, "updated": datetime.datetime.now()}