import shlex

FIELD_CODES = {
    "%f",
    "%F",
    "%u",
    "%U",
    "%i",
    "%c",
    "%k",
    "%%"
}


def build_command(exec_line):
    if not exec_line:
        return None, []

    try:
        parts = shlex.split(exec_line)
    except ValueError:
        return None, []

    result = []

    for part in parts:
        if part in FIELD_CODES:
            if part == "%%":
                result.append("%")
            continue

        if part.startswith("@@"):
            continue

        result.append(part)

    if not result:
        return None, []

    return result[0], result[1:]