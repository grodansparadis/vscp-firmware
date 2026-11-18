#!/usr/bin/env python3
"""Convert a VSCP JSON MDF to the XML MDF form read by vscp/src/vscp/common/mdf.cpp.

usage: json2xml.py in.json out.xml
"""
import json
import sys
from xml.sax.saxutils import escape, quoteattr


def s(v):
    return '' if v is None else str(v)


class W:
    def __init__(self):
        self.lines = []
        self.depth = 0

    def line(self, text):
        self.lines.append('  ' * self.depth + text)

    def open(self, tag, attrs=()):
        self.line(f'<{tag}{fmt_attrs(attrs)}>')
        self.depth += 1

    def close(self, tag):
        self.depth -= 1
        self.line(f'</{tag}>')

    def leaf(self, tag, text, attrs=()):
        if text is None:
            return
        self.line(f'<{tag}{fmt_attrs(attrs)}>{escape(s(text))}</{tag}>')

    def empty(self, tag, attrs=()):
        self.line(f'<{tag}{fmt_attrs(attrs)} />')


def fmt_attrs(attrs):
    return ''.join(f' {k}={quoteattr(s(v))}' for k, v in attrs if v is not None)


def lang_items(value):
    """Yield (lang, text) from {"en": ..} / [{"en": ..}, ..] / "text"."""
    if value is None:
        return
    if isinstance(value, str):
        yield 'en', value
    elif isinstance(value, dict):
        yield from value.items()
    elif isinstance(value, list):
        for item in value:
            yield from lang_items(item)


def texts(w, obj):
    for key in ('description', 'infourl'):
        for lang, text in lang_items(obj.get(key)):
            w.leaf(key, text, [('lang', lang)])


def bit(w, b):
    attrs = [(k, b.get(k)) for k in ('name', 'pos', 'width', 'default', 'min', 'max', 'access')]
    w.open('bit', attrs)
    texts(w, b)
    for v in b.get('valuelist') or []:
        value(w, v)
    w.close('bit')


def value(w, v):
    w.open('item', [('value', v.get('value')), ('name', v.get('name'))])
    texts(w, v)
    w.close('item')


def valuelist(w, obj):
    items = obj.get('valuelist') or []
    if items:
        w.open('valuelist')
        for v in items:
            value(w, v)
        w.close('valuelist')


def convert(m):
    w = W()
    w.lines.append('<?xml version="1.0" encoding="UTF-8"?>')
    w.lines.append('<!-- Generated from blinky0.json. Edit the JSON MDF and regenerate. -->')
    w.open('vscp')
    w.open('module')
    for k in ('name', 'copyright', 'level', 'model', 'version', 'changed', 'buffersize'):
        w.leaf(k, m.get(k))
    texts(w, m)

    man = m.get('manufacturer')
    if man:
        w.open('manufacturer')
        w.leaf('name', man.get('name'))
        addr = man.get('address')
        if addr:
            w.open('address')
            for k in ('street', 'town', 'city', 'postcode', 'state', 'region', 'country'):
                w.leaf(k, addr.get(k))
            w.close('address')
        for k in ('telephone', 'fax', 'email', 'web', 'social'):
            for item in man.get(k) or []:
                w.leaf(k, item.get('number') or item.get('address') or item.get('url'),
                       [('description', item.get('description'))])
        w.close('manufacturer')

    boot = m.get('boot')
    if boot:
        w.open('boot')
        for k in ('algorithm', 'blocksize', 'blockcount'):
            w.leaf(k, boot.get(k))
        w.close('boot')

    files = m.get('files') or {}
    groups = [files] if isinstance(files, dict) else files
    if any(entries for g in groups for entries in g.values()):
        w.open('files')
        for group in groups:
            for kind, entries in group.items():
                for f in entries:
                    attrs = [(k, f.get(k)) for k in ('name', 'url', 'format', 'lang', 'date')]
                    w.open(kind, attrs)
                    texts(w, f)
                    w.close(kind)
        w.close('files')

    regs = m.get('registers') or []
    if regs:
        w.open('registers')
        for r in regs:
            keys = ('name', 'page', 'offset', 'span', 'width', 'type', 'access', 'default', 'min', 'max',
                    'fgcolor', 'bgcolor')
            w.open('reg', [(k, r.get(k)) for k in keys])
            texts(w, r)
            for b in r.get('bit') or []:
                bit(w, b)
            valuelist(w, r)
            w.close('reg')
        w.close('registers')

    rvars = m.get('remotevars') or []
    if rvars:
        w.open('remotevars')
        for v in rvars:
            keys = ('name', 'type', 'default', 'page', 'offset', 'bitpos', 'access', 'fgcolor', 'bgcolor')
            w.open('remotevar', [(k, v.get(k)) for k in keys])
            texts(w, v)
            valuelist(w, v)
            w.close('remotevar')
        w.close('remotevars')

    alarm = m.get('alarm') or []
    if alarm:
        w.open('alarm')
        for b in alarm:
            bit(w, b)
        w.close('alarm')

    dm = m.get('dmatrix')
    if dm:
        w.open('dmatrix')
        w.leaf('level', dm.get('level'))
        w.empty('start', [('page', dm.get('start-page')), ('offset', dm.get('start-offset'))])
        w.leaf('rowcnt', dm.get('rowcount', dm.get('rowcnt')))
        w.leaf('rowsize', dm.get('rowsize'))
        for a in dm.get('action') or []:
            w.open('action', [('code', a.get('code')), ('name', a.get('name'))])
            texts(w, a)
            for p in a.get('param') or []:
                w.open('param', [(k, p.get(k)) for k in ('name', 'offset', 'min', 'max')])
                texts(w, p)
                valuelist(w, p)
                for b in p.get('bit') or []:
                    bit(w, b)
                w.close('param')
            w.close('action')
        w.close('dmatrix')

    events = m.get('events') or []
    if events:
        w.open('events')
        for e in events:
            keys = ('name', 'class', 'type', 'priority', 'direction')
            w.open('event', [(k, e.get(k)) for k in keys])
            texts(w, e)
            for d in e.get('data') or []:
                w.open('data', [('name', d.get('name')), ('offset', d.get('offset'))])
                texts(w, d)
                valuelist(w, d)
                for b in d.get('bit') or []:
                    bit(w, b)
                w.close('data')
            w.close('event')
        w.close('events')

    w.close('module')
    w.close('vscp')
    return '\n'.join(w.lines) + '\n'


def main():
    src, dst = sys.argv[1], sys.argv[2]
    with open(src, encoding='utf-8') as f:
        doc = json.load(f)
    known = {'name', 'copyright', 'level', 'model', 'version', 'changed', 'buffersize', 'description',
             'infourl', 'manufacturer', 'boot', 'files', 'registers', 'remotevars', 'alarm', 'dmatrix',
             'events'}
    unknown = set(doc['module']) - known
    if unknown:
        sys.exit(f'unhandled module keys: {sorted(unknown)}')
    with open(dst, 'w', encoding='utf-8') as f:
        f.write(convert(doc['module']))


if __name__ == '__main__':
    main()
