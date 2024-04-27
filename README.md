# apogee2drawio

`apogee2drawio` is a Python package designed to process XML dumps of the diploma decomposition from Apogée and transform them into diagrams in the DrawIO format.

## Installation

You can install `apogee2drawio` either directly from the Git repository or from a tarball. 

### Installation from Git

```bash
pip install git+https://gitlab.dsi.universite-paris-saclay.fr/pavel.kalouguine/apogee2drawio
```

### Installation from Tarball

Download the tarball from https://gitlab.dsi.universite-paris-saclay.fr/pavel.kalouguine/apogee2drawio (<kbd>Code | tar.gz</kbd>) and run 

```bash
pip install /path/to/apogee2drawio-main.tar.gz
```

## About DrawIO
DrawIO is a free and open source diagramming tool that allows users to create diagrams, flowcharts, and other visual representations. 

You can download the desktop version of DrawIO on its [official website](https://www.drawio.com/) or use the [online application](https://app.diagrams.net/). 

## Usage
Once apogee2drawio is installed, you can use it from the command line to process XML dumps from Apogée and generate DrawIO diagrams. 

### Example Usage
```bash
python -m apogee2drawio.process -in input.xml -out output.drawio
```
This command will process the XML file `input.xml` and generate a DrawIO diagram saved as `output.drawio`. The generated diagram will display Apogée codes for ELPs. A detailed description of each ELP will appear as a tooltip when hovering the mouse over the corresponding item."

```bash
python -m apogee2drawio.process -in input.xml -out output.drawio -s description
```
The generated diagram will display detailed descriptions of ELPs, with Apogée codes appearing as tooltips.

```bash
python -m apogee2drawio.process -in input.xml -out output.drawio -s both
```
Both the Apogée codes and the details of ELPs will be shown.

Command Line Options
- `-in`: Path to the input XML file containing the diploma decomposition from Apogée.
- `-out`: Path to save the output DrawIO diagram.
- `-s`: Information to display (`code_apogee`, `description` or `both`). The default is `code_apogee`.

The generated diagram can then be opened in the DrawIO application to adjust the layout of blocks. (The topology of links is conserved as blocks are moved across the page.) You can save the new layout and keep the diagram in the DrawIO format or export it, for instance, as a PDF. Note, however, that exported diagrams lose the tooltip information.

## Support
For any questions, issues, or feedback, please open an issue on the GitHub repository. We welcome contributions and suggestions to improve this tool.

