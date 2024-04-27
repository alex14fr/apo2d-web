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
    y=TOP_POS
    for i in range(len(graph)):
        level=graph[i]
        # Draw blocks:
        x=LEFT_POS
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
                drawlink(root, src=src, dst=dst, label=label)
    write_mxfile(mxfile, outfile)
