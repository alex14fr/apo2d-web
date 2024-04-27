from .diagram_writer import makemxfile, drawblock, drawlink, write_mxfile, height_block, width_block, ShowOption
from .apogee_parser import parse_doc
from .data_filter import prepare_graph

LAYER_SPACING=80
HORIZONTAL_SPACING=40
TOP_POS=120
LEFT_POS=120

def make_diagram(infile, outfile, to_show=ShowOption.CODE_APOGEE):
    mxfile, root=makemxfile()
    data=parse_doc(infile)
    graph=prepare_graph(data)
    widths_level=[]
    for i in range(len(graph)):
        widths_level.append(0)
        blocks=graph[i]['blocks']
        for k in blocks:
            widths_level[i]+=width_block(blocks[k], to_show)
    y=TOP_POS
    for i in range(len(graph)):
        level=graph[i]
        # Draw blocks:
        x=LEFT_POS-widths_level[i]*.5
        blocks=level['blocks']
        h=0
        for k in blocks:
            block=blocks[k]
            w=width_block(block, to_show)
            drawblock(root, blocks[k], (x, y), to_show)
            x+=(w+HORIZONTAL_SPACING)
            h=max(h, height_block(block))
        y+=(h+LAYER_SPACING)
        # Draw links:
        uplinks=level['uplinks']
        child_number=0
        west_east_cutoff=int((1+len(uplinks))*.5) #favor west for odd number of children
        for u in uplinks:
            if i==0:
                raise ValueError(f'Uplink found on the top level')
            prev_level=graph[i-1]
            upper_elps=prev_level['active_elps'][u['parent']]
            for ue in upper_elps:
                upper_block=prev_level['blocks'][ue['list']]
                src=(upper_block, ue['pos'])
                dst=level['blocks'][u['child']]
                # TODO: add a label to the link if necessary
                label=None
                if 'min' in u and 'max' in u:
                    if u['min']!=None and u['max']!=None:
                        label=f"{u['min']}-{u['max']}"
                sourcePort=("west" if child_number<west_east_cutoff  else "east")
                drawlink(root, src=src, dst=dst, label=label, sourcePort=sourcePort)
            child_number+=1
    write_mxfile(mxfile, outfile)
