import yaml
import json
import re
import sys

link_prefix = ''
if len(sys.argv) > 2:
    link_prefix = sys.argv[2]


def parse_schema(schema, definitions):
    if '$ref' in schema:
        ref = schema['$ref']
        match = re.match('^#/definitions/(.*)', ref)
        if match:
            type = ref[match.regs[1][0]:match.regs[1][1]]
            example = ''
            try:
                example = definitions[type]['example']
            except:
                pass
            return '[{}]({}{})'.format(type, link_prefix, type.lower()), example
        else:
            return ref, ''
    else:
        example = ''
        if 'example' in schema:
            example = schema['example']
        if not 'type' in schema:
            return '', example

        type = schema['type']
        if type == 'number' and 'format' in schema:
            type = schema['format']
        elif type == 'array':
            if not 'items' in schema:
                exit(-1)
            (type, example) = parse_schema(schema['items'], definitions)
            type = '{}[]'.format(type)
            example = '[{}]'.format(example)
        elif type == 'string':
            example = '"{}"'.format(example)

        return type, example


def fix_endpoint_name(endpoint):
    return endpoint.replace('${', ':').replace('}', '').lower()


def create_filename(method, endpoint):
    return '{}{}'.format(
        method.lower(),
        fix_endpoint_name(endpoint)).replace('/', '-').replace(':', '')


def create_summary(paths, definitions):
    str = '<details open>\n'
    str += '<summary>Overview</summary>\n\n'
    str += '[API Reference](api_reference)\n'
    str += '* Endpoints\n'
    for path in paths:
        methods = paths[path]
        for method in methods:
            filename = create_filename(method, path)
            str += '  * [{} {}]({}{})\n'.format(method.upper(),
                                                fix_endpoint_name(path), link_prefix, filename)
    str += '* Definitions\n'
    for definition in definitions:
        str += '  * [{}]({}{})\n'.format(definition,
                                         link_prefix, definition.lower())
    str += '</details>\n\n'
    return str


def parse_method(doc_file, summary, method, endpoint, attributes, definitions):
    filename = create_filename(method, endpoint)

    doc_file.write('### [{} {}]({}{})\n'.format(
        method.upper(), fix_endpoint_name(endpoint), link_prefix, filename))

    endpoint_file = open('{}.md'.format(filename), 'w')
    endpoint_file.write(summary)
    endpoint_file.write('# {} {}\n\n'.format(
        method.upper(), fix_endpoint_name(endpoint)))
    endpoint_file.write(attributes['summary'] + '\n\n')
    endpoint_file.write('### Parameters\n')
    parameters = attributes['parameters']
    if len(parameters) > 0:
        endpoint_file.write(
            '|Name|Type|Required|Description|Example|\n')
        endpoint_file.write('|---|---|---|---|---|\n')
        for parameter in parameters:
            (type, example) = parse_schema(parameter, definitions)
            if len(example) > 0:
                example = '`{}`'.format(example)
            endpoint_file.write('|{}|{}|{}|{}|{}|\n'.format(
                parameter['name'], type, parameter['required'], parameter['description'], example))
    else:
        endpoint_file.write('None\n')

    endpoint_file.write('### Response\n\n')
    produces = attributes['produces']
    if len(produces) == 1:
        endpoint_file.write('Produces: {}\n\n'.format(produces[0]))
    else:
        endpoint_file.write('Produces: {}\n\n'.format(produces))

    responses = attributes['responses']
    if len(responses) > 0:
        endpoint_file.write(
            '|Code|Description|Type|Example|\n')
        endpoint_file.write('|---|---|---|---|\n')
        for response_code in responses:
            response = responses[response_code]
            (type, example) = parse_schema(response['schema'], definitions)
            if len(example) > 0:
                example = '`{}`'.format(example)
            endpoint_file.write('|{}|{}|{}|{}|\n'.format(
                response_code, response['description'], type, example))
    else:
        endpoint_file.write('None\n')


def parse_definition(doc_file, summary, definition, properties, definitions):
    filename = definition.lower()

    doc_file.write('### [{}]({}{})\n'.format(
        definition, link_prefix, filename))

    definition_file = open('{}.md'.format(filename), 'w')
    definition_file.write(summary)
    definition_file.write('# {}\n\n'.format(definition))

    definition_type = properties['type']
    if not definition_type == 'object':
        exit(-1)

    definition_file.write('## Properties\n\n')
    object_properties = properties['properties']
    definition_file.write(
        '|Name|Type|Description|\n')
    definition_file.write('|---|---|---|\n')
    for object_property_name in object_properties:
        object_property = object_properties[object_property_name]
        description = ''
        if 'description' in object_property:
            description = object_property['description']
        (type, example) = parse_schema(object_property, definitions)
        definition_file.write('|{}|{}|{}|\n'.format(
            object_property_name, type, description))

    definition_file.write('## Example\n\n')
    if 'example' in properties:
        definition_file.write(
            '```json\n{}\n```\n'.format(
                json.dumps(properties['example'], indent=4)))
    else:
        definition_file.write('There is no example yet.')


with open(sys.argv[1]) as file:
    with open(r'api_reference.md', 'w') as doc_file:
        data = yaml.load(file, Loader=yaml.BaseLoader)

        if not data['swagger'] == "2.0":
            exit(-1)

        doc_file.write('# ' + data['info']['title'] + '\n')
        doc_file.write('*Version ' + data['info']['version'] + '*\n\n')
        doc_file.write(data['info']['description'] + '\n\n')

        paths = data['paths']
        definitions = data['definitions']
        summary = create_summary(paths, definitions)

        doc_file.write('## Endpoints\n')

        for path in paths:
            methods = paths[path]
            for method in methods:
                parse_method(doc_file, summary, method, path,
                             methods[method], definitions)

        doc_file.write('## Definitions\n')
        for definition in definitions:
            parse_definition(doc_file, summary, definition,
                             definitions[definition], definitions)
