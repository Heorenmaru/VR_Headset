import pandas as pd

# Загрузка данных из CSV файла
input_file = 'tc358870.csv'  # Укажите путь к вашему входному файлу
output_file = 'output.csv'  # Укажите путь к выходному файлу

# Чтение CSV файла
data = pd.read_csv(input_file)

# Создание списка для хранения обработанных строк
processed_data = []

# Переменные для хранения текущего состояния
current_name = None
current_start = None
current_ack = None
current_stop = None
event_type = None
dat = []

# Обработка каждой строки данных
for index, row in data.iterrows():
    if row['type'] == 'start':
        
        
        current_name = row['name']
        current_start = 'start'
        current_ack = None  # Сбрасываем ack при новом старте
        current_stop = None  # Сбрасываем stop при новом старте
        dat = []

    elif row['type'] == 'address':
        address = row['address'] if pd.notna(row['address']) else ''
        event_type = row['read']
        if event_type != True:
            event_type = 'W'
        else:
            event_type = 'R'
        current_ack = row['ack']
        
    elif row['type'] == 'data':
        data_value = row['data'] if pd.notna(row['data']) else ''
        dat.append(data_value)
        
    elif row['type'] == 'stop':
        current_stop = 'stop'
        raddr = ''
        rdat = ''
        for i in range(len(dat)):
            if i == 0:
                raddr = dat[i]
            if i == 1: 
                raddr += dat[i][2:]
            if i == 2:
                rdat = dat[i]
            if i > 2:
                rdat += dat[i][2:]

        # Добавление строки в список обработанных данных
        processed_data.append({
            'name': current_name,
            'RW': event_type,
            'chipaddr': address,
            'addr': raddr,
            'dat' : rdat,
            'ack' : current_ack
        })

# Преобразование списка в DataFrame
output_df = pd.DataFrame(processed_data)

# Сохранение результата в новый CSV файл с нужным форматом
output_df.to_csv(output_file, sep=';', index=False, header=True)

print(f'Данные успешно сохранены в {output_file}')