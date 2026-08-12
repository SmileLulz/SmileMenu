import shlex
import re

# Standard Freedesktop field codes (%f, %F, %u, %U, %i, %c, %k, %v, %m)
FIELD_CODE_RE = re.compile(r'%[fFuUiIckvm]')

def build_command(exec_line):
    if not exec_line:
        return None, []

    try:
        parts = shlex.split(exec_line)
    except ValueError:
        # Fallback simple split if quotes are unclosed
        parts = exec_line.split()

    result = []

    for part in parts:
        if part == "%%":
            result.append("%")
            continue

        if part.startswith("@@"):
            continue

        cleaned = FIELD_CODE_RE.sub("", part)

        if cleaned:
            result.append(cleaned)

    if not result:
        return None, []

    return result[0], result[1:]
