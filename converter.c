#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "lib/cJSON.h"

extern int convert(int a, int b);

/**
 * @brief Guarda en el archivo output.json la respuesta de la llamada a la API
 *
 * @param url de la llamada a la API
 * @return 0 si funcionó de manera correcta o 1 si hubo un error
 */

int get_value(char *url)
{
  CURL *curl;
  CURLcode res;
  struct curl_slist *headers = NULL;
  FILE *fp;

  curl = curl_easy_init(); // Inicializa la estructura curl que contiene
                           // información sobre la transferencia como el url
                           // y que hacer una vez que se obtiene la respuesta.
  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, url);                         // Se asigna el url
    headers = curl_slist_append(headers, "accept: application/json"); // header que pide la API
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Si el link redirige sigue la redirección

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite); // Opción para escribir los datos en el archivo

    fp = fopen("output.json", "w"); // Abre el archivo para escritura

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp); // Selecciona el archivo sobre el que se va a escribir

    res = curl_easy_perform(curl); // Hace la llamada a la api y la escritura

    fclose(fp); // Cierra el archivo

    if (res != CURLE_OK) // Si res = resultado funcionó, salta
    {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      return 1;
    }

    curl_slist_free_all(headers); // Elimina el header pedido por la api
    curl_easy_cleanup(curl);      // Limpia la estructura curl
    return 0;                     // Retorna cero indicando éxito
  }
  printf("Error en CURL init\n");
  return 1;
}

/**
 * @brief Lee el archivo "output.json" y devuelve el valor de la criptomoneda
 * en $ARS.
 *
 * @param coin nombre de la moneda
 * @return int valor de la criptomoneda en $ARS
 */

int read_output(char *coin)
{
  int usd_value = 0;
  // Open the file for reading
  FILE *fp = fopen("output.json", "r");
  if (fp == NULL)
  {
    fprintf(stderr, "Error opening file\n");
    return 0;
  }

  // Read the contents of the file into a string
  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);             // Junto con fseek determinan el tamaño del archivo
  rewind(fp);                             // Se retorna fp a su valor inicial
  char *json_str = malloc(file_size + 1); // Creo un string de tamaño del archivo + 1
  if (json_str == NULL)
  {
    fprintf(stderr, "Error allocating memory\n");
    free(json_str);
    return 0;
  }
  fread(json_str, 1, file_size, fp); // Se lee el archivo de a un byte y se lo guarda en el string
  json_str[file_size] = '\0';        // Se coloca un NULL como último caracter
  fclose(fp);                        // Se cierra el archivo

  // Parse the JSON string
  cJSON *root = cJSON_Parse(json_str); // Guarda el string en formato cjson
  if (root == NULL)
  {
    fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
    free(json_str);
    return 0;
  }

  // Extract the "ars" value from the JSON object
  cJSON *usd_obj = cJSON_GetObjectItemCaseSensitive(cJSON_GetObjectItem(root, coin), "usd"); // Extrae el valor del precio de la cripto en dolares
  if (cJSON_IsNumber(usd_obj))                                                               // Si el valor es un numero
  {
    usd_value = usd_obj->valueint; // lo guarda en la variable a retornar
  }
  else
  {
    fprintf(stderr, "Error getting \"usd\" value from JSON\n");
    return 0;
  }

  cJSON_Delete(root); // Elimina el cjson creado
  free(json_str);     // Libera el espacio de memoria ocupaso por el string

  return usd_value;
}

void print_values(int value_in_usd, char *coin_name)
{
  double USD_ARS = 216;
  double USD_EUR = 0.91;

  // Multiplica los valores por 100 para no tener decimales
  int value_in_ars = convert(value_in_usd, (int)(USD_ARS * 100));
  int value_in_eur = convert(value_in_usd, (int)(USD_EUR * 100));

  printf("Valor de 1 %s en $USD = $%d\n", coin_name, value_in_usd);
  printf("Valor de 1 %s en ARS = $%d\n", coin_name, value_in_ars);
  printf("Valor de 1 %s en EUR = $%d\n", coin_name, value_in_eur);
}

int main(void)
{
  char *url_bitcoin = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd";
  char *url_monero = "https://api.coingecko.com/api/v3/simple/price?ids=monero&vs_currencies=usd";
  char *url_ethereum = "https://api.coingecko.com/api/v3/simple/price?ids=ethereum&vs_currencies=usd";

  int coin_id;
  int value_in_usd;

  while (1)
  {
    printf("Ingrese la criptomoneda:\n 1. Bitcoin\n 2. Monero\n 3. Ethereum\n");
    scanf("%d", &coin_id); // Pide la moneda a ingresar

    switch (coin_id)
    {
    case 1:
      if (get_value(url_bitcoin))  // Llamada a la api
        return 1;
      value_in_usd = read_output("bitcoin"); // Valor en $ARS de un bitcoin
      if (value_in_usd == 0)
        return 1;
      print_values(value_in_usd, "BTC");
      break;

    case 2:
      if (get_value(url_monero))  // Llamada a la api
        return 1;
      value_in_usd = read_output("monero"); // Valor en $ARS de un monero
      if (value_in_usd == 0)
        return 1;
      print_values(value_in_usd, "XMR");
      break;

    case 3:
      if (get_value(url_ethereum))  // Llamada a la api
        return 1;
      value_in_usd = read_output("ethereum"); // Valor en $ARS de un ether
      if (value_in_usd == 0)
        return 1;
      print_values(value_in_usd, "ETH");
      break;

    default:
      printf("El valor ingresado no corresponde con ninguna criptomoneda\n");
      break;
    }
  }

  return 0;
}