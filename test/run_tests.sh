#!/bin/bash
pytest --count=2 --repeat-scope=session -x --showlocals /tests -s
