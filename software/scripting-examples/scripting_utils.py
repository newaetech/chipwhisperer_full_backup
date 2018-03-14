from terminaltables import AsciiTable


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
        data.insert(0, self.counter)
        self.data.append(data)
        self.counter += 1

    def display(self):
        table = AsciiTable(self.data)
        print(table.table)