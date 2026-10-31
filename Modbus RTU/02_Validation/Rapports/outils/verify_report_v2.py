#!/usr/bin/env python3
"""Independent PDF validation: text, page bounds, image pixels, renders and source hashes."""
from pathlib import Path
import sys,argparse,hashlib,json
try:import fitz
except ImportError:
 sys.path.insert(0,'/home/lolo/dev/msm/cache/r5-bv-pk-pdf-deps');import fitz
ROOT=Path(__file__).resolve().parents[4]
PDF=ROOT/'Modbus RTU/02_Validation/Rapports/MSM_TR2_Rapport_capacites_vibratoires_31-10-2026.pdf'
P=ROOT/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261031-R5-ANALYSE-PC-MACHINE-6H'

def sha(b):return hashlib.sha256(b).hexdigest()
def rgb(pix):return fitz.Pixmap(pix,0) if pix.alpha else pix

def verify(out):
 out=Path(out);out.mkdir(parents=True,exist_ok=False);data=PDF.read_bytes();assert data.startswith(b'%PDF-')
 d=fitz.open(PDF);assert len(d)==10 and not d.is_encrypted
 first=d[0].get_text().lower()
 for s in ['plancher du pc machine','occupé en permanence en navigation','4,0','4,5','5,0','1,224','2,082','2,775','60 secondes','pas une crête instantanée','grade 1','grade 2','grade 3']:
  assert s in first,s
 assert 'conforme' not in first and '95%' not in first
 images=[];pages=[];outside=[]
 for i,p in enumerate(d):
  text=p.get_text();assert len(text.strip())>100
  h,w=p.rect.height,p.rect.width;assert (abs(w-595.276)<.1 and abs(h-841.89)<.1) or (abs(w-841.89)<.1 and abs(h-595.276)<.1)
  # Render every page, independent of the PDF text extraction.
  pix=p.get_pixmap(matrix=fitz.Matrix(1.5,1.5),alpha=False);pix.save(out/f'page-{i+1:02d}.png')
  for block in p.get_text('dict')['blocks']:
   if block['type']==0:
    for line in block['lines']:
     for s in line['spans']:
      box=fitz.Rect(s['bbox']);limit=fitz.Rect(-.5,-.5,w+.5,h+.5)
      if not limit.contains(box):outside.append(dict(page=i+1,text=s['text'],bbox=list(box)))
  footer=p.get_textbox(fitz.Rect(0,h-19,w,h)).strip() if i in [3,4,5] else p.get_textbox(fitz.Rect(w-85,h-43,w-50,h-15)).strip()
  assert str(i+1) in footer,(i+1,footer)
  pages.append(dict(page=i+1,width=w,height=h,text_chars=len(text),image_count=len(p.get_images()),links=len(p.get_links()),render_rgb_sha256=sha(pix.samples)))
  if i in [3,4,5]:
   name={3:'02_spectrogram.png',4:'03_spectra_ridges.png',5:'04_ridge_power.png'}[i];original=rgb(fitz.Pixmap(P/name));expected=sha(original.samples);found=[]
   for img in p.get_images():
    current=rgb(fitz.Pixmap(d,img[0]))
    if current.width==original.width and current.height==original.height and sha(current.samples)==expected:
     rects=p.get_image_rects(img[0]);assert len(rects)==1 and p.rect.contains(rects[0]);r=rects[0];assert abs(r.width/r.height-original.width/original.height)<1e-5
     found.append(dict(xref=img[0],rect=list(r),dpi=original.width*72/r.width))
   assert len(found)==1,('original image absent or changed',name)
   images.append(dict(page=i+1,source=name,source_png_sha256=sha((P/name).read_bytes()),native_width=original.width,native_height=original.height,source_RGB_sha256=expected,extracted_pixels_identical=True,occurrences=found))
 assert not outside,outside
 text='\n'.join(p.get_text() for p in d);(out/'text.txt').write_text(text)
 assert 'Le spectrogramme original ne figure pas' not in text
 assert '34,40' in text and '145,64' in text # Technical measurements retained from original vector page.
 assert '120 s' in text and '2,458' in text
 assert 'STEVAL-MKI208V1' in text and 'STM32U575' in text and 'microSD' in text
 proof=dict(pdf_sha256=sha(data),bytes=len(data),pages=len(d),embedded_original_figures=len(images),figures=images,first_page_required_text=True,no_page_blank=True,text_bounds_ok=True,all_pages_rendered=True,page_details=pages,pymupdf=fitz.VersionBind,checks='No signal computation, only published summaries/display rounding and original images/vector pages')
 (out/'verification.json').write_text(json.dumps(proof,indent=2)+'\n');print(json.dumps({k:v for k,v in proof.items() if k not in ['figures','page_details']}))
if __name__=='__main__':
 a=argparse.ArgumentParser();a.add_argument('output');o=a.parse_args();verify(o.output)
