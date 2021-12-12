#! /usr/bin/env python3

import mupdf

import os
import sys


assert len(sys.argv) == 7
filename, page_num, zoom, rotate, output, needle = sys.argv[1:]
page_num = int(page_num)
zoom = int(zoom)
rotate = int(rotate)

document = mupdf.Document(filename)

print('')
print(f'Document {filename} has {document.count_pages()} pages.')
print('')
print(f'Metadata Information:')
print(f'mupdf.metadata_keys={mupdf.metadata_keys}')
for key in mupdf.metadata_keys:
    value = document.lookup_metadata(key)
    print(f'    {key}: {value!r}')
print('')

depth = 0
it = mupdf.OutlineIterator(document)
while 1:
    item = it.outline_iterator_item()
    if item.valid():
        print(f'    {" "*4*depth}{item.title()}')
    r = it.outline_iterator_down()
    if r >= 0:
        depth += 1
    if r < 0:
        r = it.outline_iterator_next()
        assert r
        if r:
            # No more items at current depth, so repeatedly go up until we
            # can go right.
            end = 0
            while 1:
                r = it.outline_iterator_up()
                if r < 0:
                    # We are at EOF. Need to break out of top-level loop.
                    end = 1
                    break
                depth -= 1
                r = it.outline_iterator_next()
                if r == 0:
                    # There are items at this level.
                    break
            if end:
                break

if page_num > document.count_pages():
    raise SystemExit(f'page_num={page_num} is out of range - {filename} has {document.count_pages()} pages')

page = document.load_page(page_num)
links = page.load_links()
if links:
    print(f'Links on page {page_num}:')
    for link in links:
        if link.m_internal:
            print(f'    extern={mupdf.is_external_link(link.uri())}: {link.uri()}')
else:
    print(f'No links on page {page_num}')

trans = mupdf.Matrix.scale(zoom / 100.0, zoom / 100.0).pre_rotate(rotate)

pixmap = page.new_pixmap_from_page(trans, mupdf.Colorspace(mupdf.Colorspace.Fixed_RGB), alpha=False)

def save_pixmap(path):
    suffix = os.path.splitext(path)[1]
    if 0: pass
    elif suffix == '.pam':   pixmap.save_pixmap_as_pam(path)
    elif suffix == '.pbm':   pixmap.save_pixmap_as_pbm(path)
    elif suffix == '.pcl':   pixmap.save_pixmap_as_pcl(path, append=0, options=mupdf.PclOptions())
    elif suffix == '.pclm':  pixmap.save_pixmap_as_pclm(path, append=0, options=mupdf.PclmOptions())
    elif suffix == '.pdfocr':pixmap.save_pixmap_as_pdfocr(path, append=0, options=mupdf.PdfocrOptions())
    elif suffix == '.pkm':   pixmap.save_pixmap_as_pkm(path)
    elif suffix == '.png':   pixmap.save_pixmap_as_png(path)
    elif suffix == '.pnm':   pixmap.save_pixmap_as_pnm(path)
    elif suffix == '.ppm':   pixmap.save_pixmap_as_ppm(path)
    elif suffix == '.ps':    pixmap.save_pixmap_as_ps(path, append=0)
    elif suffix == '.psd':   pixmap.save_pixmap_as_psd(path)
    elif suffix == '.pwg':   pixmap.save_pixmap_as_pwg(path, append=0, pwg=mupdf.PwgOptions())
    else:
        raise Exception(f'Unrecognised output format: {path}')
save_pixmap(output)
hit_quads = page.search_page(needle, max=16)
print(f'search text {needle!r} found {len(hit_quads)} on the page')
for hit_quad in hit_quads:
    pixmap.invert_pixmap_rect(hit_quad.rect_from_quad().irect_from_rect())
save_pixmap(f'dl-{output}')

print('finished')
