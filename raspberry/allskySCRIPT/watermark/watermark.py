#!/usr/bin/env python

from PIL import Image, ImageDraw, ImageFont

def main():
    # Open the original image
    main = Image.open("input_nb.png")
    # Adding an alpha channel to the main image
    main = main.convert("RGBA")

    # Open the watermark image
    watermark = Image.open("watermark_option4.png")
    # Adding an alpha channel to the main image
    watermark = watermark.convert("RGBA")

    # Get an ImageDraw object so we can draw on the image
    waterdraw = ImageDraw.ImageDraw(watermark, "RGBA")
    # get a font
    fnt = ImageFont.truetype('good times rg.ttf', 12)
    # Place the text at (10, 10) in the upper left corner. Text will be white.
    waterdraw.text((5, 25), "2017-08-03", font=fnt, fill=(255,255,255,255))
    waterdraw.text((5, 50), "1s", font=fnt, fill=(255,255,255,255))

    # Make a final composite image
    result = Image.alpha_composite(main, watermark)

    # Paste the watermark (with alpha layer) onto the original image and save it
    #main.paste(watermark, None, watermark)
    result.save("watermarked_option4.jpg", "JPEG")

if __name__ == '__main__':
    main()
