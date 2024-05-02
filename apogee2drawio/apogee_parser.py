import xml.etree.ElementTree as etree
import pprint

def parse_doc(filename):
    doc = etree.parse(filename)
    rv=[]
    # Treat the topmost block
    gclse=doc.find('./LIST_G_COD_DIP/G_COD_DIP/LIST_G_COD_LSE/G_COD_LSE')
    clse=gclse.find('COD_LSE')
    t=gclse.find('TYP_LSE1')
    topmost={'parent':None, 'list':{'code':clse.text, 'type':t.text}, 'items':[]}
    lgcelp=gclse.find('LIST_G_COD_ELP')
    gcelps=lgcelp.findall('G_COD_ELP')
    for gcelp in gcelps:
        celp=gcelp.find('COD_ELP')
        lnel=gcelp.find('LIC_NEL')
        lelp=gcelp.find('LIC_ELP')
        sus=gcelp.find('TEM_SUS_ELP')
        item={'code':celp.text, 'type':lnel.text, 'name':lelp.text}
        if sus.text == 'O':
            item['suspended']=True
        topmost['items'].append(item)
    rv.append([topmost])
    # Treat other blocks
    levels = doc.findall('./LIST_G_NIVEAU/G_NIVEAU')
    for g in levels:
        level=g.find('NIVEAU')
        n=int(level.text)
        if n!=len(rv)+1:
            raise ValueError(f'Expecting level {len(rv)+1}, got {n}')
        elements=g.findall('LIST_G_COD_ELP_PERE1/G_COD_ELP_PERE1')
        blocks=[]
        for e in elements:
            parent=e.find('COD_ELP_PERE1')
            l=e.find('COD_LSE2')
            y=e.find('LIST_G_COD_LSE1/G_COD_LSE1/TYP_LSE')
            nmin=e.find('LIST_G_COD_LSE1/G_COD_LSE1/NBR_MIN_ELP_OBL_CHX')
            nmax=e.find('LIST_G_COD_LSE1/G_COD_LSE1/NBR_MAX_ELP_OBL_CHX')
            block={'parent':parent.text, 'list':{'code':l.text, 'type':y.text, 'min':nmin.text, 'max':nmax.text}, 'items':[]}
            children=e.findall('LIST_G_COD_ELP_FILS/G_COD_ELP_FILS')
            for c in children:
                child=c.find('COD_ELP_FILS')
                child_type=c.find('LIST_G_COD_ELP1/G_COD_ELP1/LIC_NEL1')
                child_name=c.find('LIST_G_COD_ELP1/G_COD_ELP1/LIC_ELP1')
                sus=c.find('LIST_G_COD_ELP1/G_COD_ELP1/TEM_SUS_ELP1')
                item={'code':child.text, 'type':child_type.text, 'name':child_name.text}
                if sus.text == 'O':
                    item['suspended']=True
                block['items'].append(item)
            blocks.append(block)
        rv.append(blocks)
    pprint.pp(rv)
    return rv
