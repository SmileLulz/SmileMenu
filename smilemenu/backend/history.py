from pathlib import Path
import json


HISTORY_FILE = (
    Path.home()
    / ".local/state/smilemenu/history.json"
)

def load_history():
    try:
        return json.loads(
            HISTORY_FILE.read_text()
        )

    except Exception:
        return {}

def save_history(history):
    HISTORY_FILE.parent.mkdir(
        parents=True,
        exist_ok=True
    )

    HISTORY_FILE.write_text(
        json.dumps(
            history,
            indent=4
        )
    )