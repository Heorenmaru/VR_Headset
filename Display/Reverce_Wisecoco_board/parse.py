
# Загрузка данных из CSV файла
input_file = 'output_ips1440x1440.csv'  # Укажите путь к вашему входному файлу
output_file = 'output.csv'  # Укажите путь к выходному файлу

# Чтение CSV файла
f = open(input_file)
data = f.readlines()
f.close()

data_fields = []
for l in data:
    if "name" in l:
        continue
    data_fields.append(l.split(',')) 


start = 0
stop = 0
read = 0
b_nb = 0
addr = []
data = []
command = []
commands = []
for d in data_fields:
    
    if d[1] == '"address"':
        start = 1
        if d[6] == 'false':
            command = []
            read = 0
            addr = []
            data = []
            command.append('W')
            b_nb = 0
        else:
            read = 1
            command.append('R')
        continue
        

    if d[1] == '"data"':
        if read == 0:
            if b_nb<2:
                addr.append(d[7].split('\n')[0])
                b_nb +=1
            else:
                data.append(d[7].split('\n')[0])
        else:
            data.append(d[7].split('\n')[0])
        continue
    

    if d[1] == '"stop"':
        a = '0x'
        for b in addr:
            a = a+ b[2:]
        d = '0x'
        for b in data:
            d = d+ b[2:]
        command.append(a)
        command.append(d)
        if b_nb>0:
            if command[1] == 'R':
                commands.append(command[1:])
            else:
                commands.append(command)

        continue
    
f=open(output_file, 'w')
for s in commands:
    comment = ''
    if s[1] == '0x8520':
        comment = '// Read Status'
    
    if '0x8C' in s[1]:
        comment = '// E-EDID'
    
    f.write(f'{s}          {comment}\n')
f.close()





# name, type,       start_time,     duration,       "ack",  "address",  "read", "data"
# "I2C","start",    0.000047083,    0.000000042,    ,,,
# "I2C","address",  0.000050458,    0.000023458,    true,   0x0F,       false,
# "I2C","start",    0.496360208,    0.000000042,    ,,,
# "I2C","address",  0.496363583,    0.0000245,      true,   0x0F,       false,
# "I2C","data",     0.496390667,    0.000024,       true,       ,            ,  0x85
# "I2C","data",     0.496417333,    0.000024083,    true,       ,           ,   0x20
# "I2C","start",    0.496443833,    0.000000042,    ,,,
# "I2C","address",  0.496447208,    0.000026792,    true,   0x0F,       true,
# "I2C","data",     0.496476458,    0.000024667,    false,      ,           ,   0x00
# "I2C","stop",     0.496503583,    0.000000042,    ,,,


exit()
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