def trivialTest(insts) -> bool:
    box = _da_convert(insts, BoxMachinePlus())

    if not _da_convert(box.is_open()):
        return False

    _da_convert(box.put("candy"))
    things = _da_convert(box.get())

    if things == "candy":
        return True
