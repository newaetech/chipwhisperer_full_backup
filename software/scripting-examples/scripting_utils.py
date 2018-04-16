from terminaltables import AsciiTable
import matplotlib.pyplot as plt

class GlitchResultsDisplay(object):
    """Helper class to display the results of the glitches in a table
    that is automatically formatted nicely
    """
    def __init__(self, headers):
        self.data = []
        headers.insert(0, '#')
        self.data.append(headers)
        self.counter = 0

    def add_data(self, data):
        """Expects list with format
        [output(string), glitch_width(float), glitch_offset(float), success(boolean)]
        """
        if isinstance(data[0], str) and isinstance(data[1], float) \
                and isinstance(data[2], float) and isinstance(data[3], bool):
            pass
        else:
            raise TypeError('Expecting data with format [output(string), glitch_width(float), glitch_offset(float), success(boolean)]')

        if not len(data) == 4:
            raise TypeError('Expecting data with length 4 but got %s' % len(data))

        data.insert(0, self.counter)
        self.data.append(data)
        self.counter += 1

    def display_table(self):
        table = AsciiTable(self.data)
        print(table.table)
