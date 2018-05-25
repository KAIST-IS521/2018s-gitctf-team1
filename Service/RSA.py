from Crypto.Util.number import bytes_to_long

N = 2434831100797744303461121761084081504647645500981453750882327082962789916361099071

e = 65537


def enc(msg):
    return hex(pow(bytes_to_long(msg), e, N))[2:-1]
