FROM python

COPY ./test-requirements.txt /test-requirements.txt
RUN pip install -r test-requirements.txt

# COPY ./test-requirements.txt /test-requirements.txt
# COPY ./conftest.py /conftest.py
# COPY ./access_node/ /access_node
COPY . /tests


# ENTRYPOINT ["pytest --count=2 --repeat-scope=session -x --showlocals /tests -s"]
ENTRYPOINT ["./tests/run_tests.sh"]
