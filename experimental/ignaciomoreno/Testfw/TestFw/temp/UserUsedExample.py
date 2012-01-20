
class Used:
    def __init__(self):
        c = 4
        e = 5
        global a
        a = 1

    def changeA(self):
        print("Globals: ", globals())
        print("Locals: ", locals())
        global a
        a = a + 1


class User:
    def __init__(self):
        b = 'string example'
        used = Used()
        used.changeA()

def TesterMethod():
    a = 0
    userObject = User()

if __name__ == '__main__':
    TesterMethod()
