import xml.etree.ElementTree as etree


WIDTH_ITEM=100
HIGHT_ITEM=26

STYLE_HEADER="swimlane;fontStyle=0;childLayout=stackLayout;horizontal=1;\
    startSize=30;horizontalStack=0;resizeParent=1;resizeParentMax=0;resizeLast=0;\
        collapsible=0;marginBottom=0;portConstraintRotation=0;rotatable=0;"
STYLE_ITEM="text;strokeColor=none;fillColor=none;align=left;verticalAlign=middle;\
    spacingLeft=4;spacingRight=4;overflow=hidden;points=[[0,0.5],[1,0.5]];\
        portConstraint=eastwest;rotatable=0;"
STYLE_SUSPENDED_ITEM="text;strokeColor=none;fillColor=none;align=left;verticalAlign=middle;\
    spacingLeft=4;spacingRight=4;overflow=hidden;points=[[0,0.5],[1,0.5]];\
        portConstraint=eastwest;rotatable=0;fontStyle=2;fontColor=#CCCCCC;"
STYLE_LINK="edgeStyle=segmentEdgeStyle;endArrow=classic;html=1;curved=0;rounded=1;\
    endSize=8;startSize=8;"
STYLE_LABEL="edgeLabel;resizable=0;html=1;align=center;verticalAlign=middle;"

# These codes are taken from Apogée's "référenciel", and shortened to three symbols 
CODE_ELP={'Semestre':'SEM', 'U.E.':'UE', 'U.F.':'UF', 'Rés. étape':'RET', 'Stage':'STG', 
          'Parcours':'PAR', 'Elt à choi':'ELC', 'BCC':'BCC', 'Année':'AN', 'Bloc':'BLC',
          'Certificat':'CRT', 'C.M.':'CM', 'Compétence':'CMP', 'Cursus':'CUR', 'ECUE':'ECU',
          'Examen':'EXA', 'Filière':'FIL', 'Matière':'MAT', 'Mémoire':'MEM', 'Module':'MOD',
          'Niveau':'NIV', 'option':'OPT', 'Période':'PER', 'Projet':'PRJ', 'Section':'SEC',
          'T.D.':'TD', 'T.P.':'TP', 'UE  nonADD':'UEF', 'UE sansnot':'USN', 'U.V.':'UV'}
CODE_LIST={'Obligatoire':'LO', 'Obligatoire à choix':'LOX', 'Facultative':'LF'}

def make_header(dl):
    return f"{dl['code']} {CODE_LIST[dl['type']]}"

def make_label(item):
    return f"{item['code']} {CODE_ELP[item['type']]}"

def make_id(block, item):
    return f"{block['list']['code']}__{item['code']}"

def height_block(block):
    return (len(block['items'])+1)*HIGHT_ITEM

def makemxfile():
    mxfile = etree.Element('mxfile', host='apogee2drawio')
    diagram = etree.Element('diagram', name='Page 1')
    mxfile.append(diagram)
    model = etree.Element('mxGraphModel', dx="1358", dy="688", grid="1", gridSize="10", guides="1", tooltips="1", connect="1", arrows="1", fold="1", page="1", pageScale="1", pageWidth="827", pageHeight="1169", math="0", shadow="0")
    diagram.append(model)
    root = etree.Element('root')
    model.append(root)
    zero = etree.Element('mxCell',id='0')
    one = etree.Element('mxCell',id='1', parent='0')
    root.append(zero)
    root.append(one)
    return (mxfile, root)

def drawblock(root, block, pos):
    (x,y)=pos
    dl=block['list']
    header=make_header(dl)
    container=etree.Element('mxCell', id=header, value=header, style=STYLE_HEADER, parent="1", vertex="1")
    h=height_block(block)
    g=etree.Element('mxGeometry', x=str(x), y=str(y), width=str(WIDTH_ITEM), height=str(h))
    g.set('as','geometry')
    container.append(g)
    root.append(container)
    count=0
    for item in block['items']:
        count+=1
        label=make_label(item)
        id_item=make_id(block, item)
        tip=item['name']
        uo=etree.Element('UserObject', label=label, tooltip=tip, id=id_item)
        cellstyle=STYLE_SUSPENDED_ITEM if 'suspended' in item else STYLE_ITEM
        cell=etree.Element('mxCell', style=cellstyle, parent=header, vertex="1")
        uo.append(cell)
        g=etree.Element('mxGeometry', y=str(HIGHT_ITEM*count), width=str(WIDTH_ITEM), height=str(HIGHT_ITEM))
        g.set('as','geometry')
        cell.append(g)
        root.append(uo)

def drawlink(root, src, dst, label=None):
    block, n=src
    source=make_label(block['items'][n])
    id_source=make_id(block, block['items'][n])
    target=make_header(dst['list'])
    id=f'{id_source}__{target}'
    container=etree.Element('mxCell', id=id, value="", style=STYLE_LINK, parent="1", 
                            source=id_source, target=target, edge="1")
    g=etree.Element('mxGeometry', width="50", height="50", relative="1")
    g.set('as','geometry')
    container.append(g)
    root.append(container)
    if label==None:
        return
    # Add a label
    container=etree.Element('mxCell', id=f'{id}_L', value=label, style=STYLE_LABEL, 
                            connectable="0", vertex="1", parent=id)
    g=etree.Element('mxGeometry', relative="1")
    g.set('as','geometry')
    p=etree.Element('mxPoint')
    p.set('as','offset')
    g.append(p)
    container.append(g)
    root.append(container)

def write_mxfile(mxfile, filename):
    tree=etree.ElementTree(mxfile)
    with open(filename, 'wb') as f:
        etree.indent(tree, space="  ", level=0)
        tree.write(f)