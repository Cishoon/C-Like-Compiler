# Define the translation mappings for tokens wrapped in <>
translation_map = {
    "Program": "程序",
    "declaration_list": "声明列表",
    "declaration": "声明",
    "var_declaration": "变量声明",
    "type_specifier": "类型指定符",
    "fun_declaration": "函数声明",
    "param_list": "参数列表",
    "param": "参数",
    "compound_stmt": "复合语句",
    "stmt_list": "语句列表",
    "stmt": "语句",
    "expression_stmt": "表达式语句",
    "expression": "表达式",
    "var": "变量",
    "simple_expression": "简单表达式",
    "additive_expression": "加法表达式",
    "term": "项",
    "factor": "因子",
    "fun_call": "函数调用",
    "argument": "实参",
    "simple_type": "简单类型",
    "addop": "加法运算符",
    "mulop": "乘法运算符",
    "relop": "关系运算符",
    "selection_stmt": "选择语句",
    "return_stmt": "返回语句",
    "iteration_stmt": "迭代语句",
    "opt_expression_stmt": "可选表达式语句",
    "bool_expression": "布尔表达式",
    "bool_term": "布尔项",
    "bool_factor": "布尔因子",
}


# Modify the function to accept additional parameters for direction and custom file paths

def translate_content(content, translation_map, direction='en_to_zh'):
    """
    Translate content based on the direction.

    Parameters:
    - content: The content to be translated.
    - translation_map: A dictionary mapping source language tokens to target language tokens.
    - direction: The direction of translation ('en_to_zh' for English to Chinese, 'zh_to_en' for Chinese to English).
    """
    if direction == 'en_to_zh':
        for english, chinese in translation_map.items():
            content = content.replace(f"<{english}>", f"<{chinese}>")
    elif direction == 'zh_to_en':
        for english, chinese in translation_map.items():
            content = content.replace(f"<{chinese}>", f"<{english}>")
    return content

def translate_file(input_filepath, output_filepath, translation_map, direction='en_to_zh'):
    """
    Translate a file from English to Chinese or vice versa based on the direction.

    Parameters:
    - input_filepath: The path to the input file.
    - output_filepath: The path to the output file.
    - translation_map: A dictionary mapping source language tokens to target language tokens.
    - direction: The direction of translation ('en_to_zh' for English to Chinese, 'zh_to_en' for Chinese to English).
    """
    try:
        with open(input_filepath, 'r', encoding='utf-8') as file:
            content = file.read()

        translated_content = translate_content(content, translation_map, direction)

        with open(output_filepath, 'w', encoding='utf-8') as file:
            file.write(translated_content)

        return "Translation successful", output_filepath
    except Exception as e:
        return str(e), None

# Example usage (the actual file paths and direction need to be provided by the user)
input_filepath = './grammar.txt'  # This should be set by the user
output_filepath = './grammar_zh.txt'  # This should be set by the user
direction = 'zh_to_en' if True else 'en_to_zh'  # or 'zh_to_en', should be set by the user

translate_file(output_filepath, input_filepath, translation_map, direction)