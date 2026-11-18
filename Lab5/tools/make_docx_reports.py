"""Перетворення готових Markdown-звітів Lab4 і Lab5 у редаговані DOCX.

Потрібні python-docx і Pillow. Запускати з будь-якої папки.
"""
from pathlib import Path
import re

from docx import Document
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.shared import Mm, Pt, RGBColor
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]


def make_report(number):
    folder = ROOT / f'Lab{number}' / 'Звіт'
    source = folder / f'Звіт_Lab{number}_Варіант_27.md'
    lines = source.read_text(encoding='utf-8-sig').splitlines()
    doc = Document()
    # Прибираємо декоративні лінії й шрифти теми стандартного шаблону Word.
    for style in doc.styles:
        for border in list(style.element.iter(qn('w:pBdr'))):
            border.getparent().remove(border)
        for fonts in style.element.iter(qn('w:rFonts')):
            for attribute in list(fonts.attrib):
                if attribute.endswith('Theme'):
                    del fonts.attrib[attribute]
    section = doc.sections[0]
    # Збережено A4 та поля наявного HTML-звіту.
    section.page_width, section.page_height = Mm(210), Mm(297)
    section.left_margin, section.right_margin = Mm(25), Mm(20)
    section.top_margin = section.bottom_margin = Mm(20)
    normal = doc.styles['Normal']
    normal.font.name = 'Times New Roman'
    normal.font.size = Pt(12)
    normal.paragraph_format.line_spacing = 1.15
    normal.paragraph_format.space_after = Pt(6)
    for name, size in [('Title', 18), ('Heading 1', 16), ('Heading 2', 13)]:
        style = doc.styles[name]
        style.font.name = 'Times New Roman'
        style.font.size = Pt(size)
        style.font.color.rgb = RGBColor(0, 0, 0)
        style.paragraph_format.keep_with_next = True
    code = doc.styles.add_style('Source Code', 1)
    code.font.name, code.font.size = 'Consolas', Pt(8.5)
    code.paragraph_format.line_spacing = 1
    code.paragraph_format.space_after = Pt(0)
    code.paragraph_format.widow_control = False
    caption = doc.styles['Caption']
    caption.font.name, caption.font.size = 'Times New Roman', Pt(11)
    caption.font.color.rgb = RGBColor(0, 0, 0)
    caption.font.italic = False
    caption.font.bold = False
    caption.paragraph_format.keep_with_next = False
    footer = section.footer.paragraphs[0]
    footer.alignment = WD_ALIGN_PARAGRAPH.CENTER
    field = OxmlElement('w:fldSimple')
    field.set(qn('w:instr'), 'PAGE')
    footer._p.append(field)
    section.different_first_page_header_footer = True
    cover = True
    in_code = False
    image_caption = None
    image_count = 0
    source_lines = []
    for line in lines:
        if line.startswith('```'):
            in_code = not in_code
            continue
        if in_code:
            doc.add_paragraph(line, style='Source Code')
            source_lines.append(line)
            continue
        if line.startswith('# ') and cover:
            cover = False
            doc.add_page_break()
        if cover:
            if line.strip() == '---':
                continue
            text = line.rstrip()
            paragraph = doc.add_paragraph(text, style='Title' if text == 'ЗВІТ' else 'Normal')
            paragraph.alignment = WD_ALIGN_PARAGRAPH.CENTER
            paragraph.paragraph_format.space_after = Pt(5)
            continue
        if not line.strip() or line.strip() == '---':
            continue
        if line.startswith('# '):
            doc.add_heading(line[2:], level=1)
        elif line.startswith('## '):
            doc.add_heading(line[3:], level=2)
        elif match := re.fullmatch(r'!\[(.*?)\]\((.*?)\)', line):
            title, path = match.groups()
            image = (folder / path).resolve()
            with Image.open(image) as bitmap:
                width, height = bitmap.size
            max_width, max_height = 165, 215
            display_width = min(max_width, max_height * width / height)
            paragraph = doc.add_paragraph()
            paragraph.alignment = WD_ALIGN_PARAGRAPH.CENTER
            paragraph.paragraph_format.keep_with_next = True
            paragraph.add_run().add_picture(str(image), width=Mm(display_width))
            paragraph = doc.add_paragraph(title, style='Caption')
            paragraph.alignment = WD_ALIGN_PARAGRAPH.CENTER
            image_caption = title
            image_count += 1
        elif image_caption and line.strip() == image_caption:
            image_caption = None
        else:
            paragraph = doc.add_paragraph(line)
            paragraph.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
    if in_code:
        raise ValueError(f'Незакритий блок коду: {source}')
    doc.core_properties.title = f'Звіт до лабораторної роботи {number} Варіант 27'
    doc.core_properties.author = 'Школа Михайло Володимирович'
    output = source.with_suffix('.docx')
    doc.save(output)
    reopened = Document(output)
    assert len(reopened.inline_shapes) == image_count
    assert [p.text for p in reopened.paragraphs if p.style.name == 'Source Code'] == source_lines
    print(f'Lab{number}: {len(reopened.paragraphs)} paragraphs, {len(source_lines)} code lines, {image_count} images')
    print(output)


if __name__ == '__main__':
    for number in (4, 5):
        make_report(number)
