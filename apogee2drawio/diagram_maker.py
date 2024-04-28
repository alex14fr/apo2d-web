from .diagram_writer import makemxfile, drawblock, drawlink, write_mxfile, height_block, width_block, ShowOption
from .apogee_parser import parse_doc
from .data_filter import prepare_graph
import random

LAYER_SPACING=80
HORIZONTAL_SPACING=40
TOP_POS=120
LEFT_POS=0

def make_diagram(infile, outfile, to_show=ShowOption.CODE_APOGEE):
    mxfile, root=makemxfile()
    data=parse_doc(infile)
    graph=prepare_graph(data)
    widths_level=[]
    for i in range(len(graph)):
        widths_level.append(-HORIZONTAL_SPACING)
        blocks=graph[i]['blocks']
        for k in blocks:
            widths_level[i]+=width_block(blocks[k], to_show)+HORIZONTAL_SPACING
    y=TOP_POS
    for i in range(len(graph)):
        level=graph[i]
        #pprint.pprint(level, indent=4, sort_dicts=False)
        blocks=level['blocks']
        uplinks=level['uplinks']
        links=[]
        # Reorder blocks
        if i==0:
            blocks_s=list(blocks.keys())
        else:
            prev_blocks_s=blocks_s
            blocks_s=[]
            prev_level=graph[i-1]
            # in order, for each parent block, and each parent item at level i-1
            for pbk in prev_blocks_s:
                pb=prev_level['blocks'][pbk]
                pipos=0
                for pi in pb['items']:
                    # search for a child at level i
                    for lnk in uplinks:
                        if lnk['parent']==pi['code']:
                            # child found
                            blocks_s.append(lnk['child'])
                            links.append({"src": (pb, pipos), \
                                "dst": blocks[lnk['child']], \
                                "label": lnk['min']+'-'+lnk['max'] \
                                    if (lnk['min']!=None and lnk['max']!=None) else None})
                    pipos+=1
            assert(len(blocks_s)==len(blocks))
        # Draw blocks:
        x=LEFT_POS-widths_level[i]*.5
        h=0
        for b in blocks_s:
            block=blocks[b]
            w=width_block(block, to_show)
            drawblock(root, block, (x, y), to_show)
            x+=(w+HORIZONTAL_SPACING)
            h=max(h, height_block(block))
        y+=(h+LAYER_SPACING)
        # Draw links:
        west_east_cutoff=int(((1 if random.uniform(0,1)<.5 else 0)+len(uplinks))*.5)
        i=0
        for link in links:
            sourcePort=("west" if i<west_east_cutoff  else "east")
            drawlink(root, src=link['src'], dst=link['dst'], label=link['label'], sourcePort=sourcePort)
            i=i+1
    write_mxfile(mxfile, outfile)
