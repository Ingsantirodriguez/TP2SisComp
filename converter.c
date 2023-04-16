#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "lib/cJSON.h"

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

  curl = curl_easy_init();
  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    headers = curl_slist_append(headers, "accept: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    /* Set the option to receive data in a callback */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);

    /* Open the file for writing */
    fp = fopen("output.json", "w");

    /* Set the file as the write target */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    /* Perform the request */
    res = curl_easy_perform(curl);

    /* Close the file */
    fclose(fp);

    /* Check for errors */
    if (res != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      return 1;
    }

    /* Cleanup */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return 0;
  }
  printf("Error en CURL init\n");
  return 1;
}

/**
 * @brief Lee el archivo "output.json" y devuelve el valor de la criptomoneda
 * en $ARS.
 *
 * @param coin nombre de la moneda
 * @return double valor de la criptomoneda en $ARS
 */

double read_output(char *coin)
{
  double ars_value = 0;
  // Open the file for reading
  FILE *fp = fopen("output.json", "r");
  if (fp == NULL)
  {
    fprintf(stderr, "Error opening file\n");
    return 0;
  }

  // Read the contents of the file into a string
  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  rewind(fp);
  char *json_str = malloc(file_size + 1);
  if (json_str == NULL)
  {
    fprintf(stderr, "Error allocating memory\n");
    free(json_str);
    return 0;
  }
  fread(json_str, 1, file_size, fp);
  json_str[file_size] = '\0';
  fclose(fp);

  // Parse the JSON string
  cJSON *root = cJSON_Parse(json_str);
  if (root == NULL)
  {
    fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
    free(json_str);
    return 0;
  }

  // Extract the "ars" value from the JSON object
  cJSON *ars_obj = cJSON_GetObjectItemCaseSensitive(cJSON_GetObjectItem(root, coin), "ars");
  if (cJSON_IsNumber(ars_obj))
  {
    ars_value = ars_obj->valuedouble;
  }
  else
  {
    fprintf(stderr, "Error getting \"ars\" value from JSON\n");
    return 0;
  }

  // Clean up
  cJSON_Delete(root);
  free(json_str);

  return ars_value;
}

int main(void)
{
  char *url_bitcoin = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=ars";
  char *url_monero = "https://api.coingecko.com/api/v3/simple/price?ids=monero&vs_currencies=ars";
  char *url_ethereum = "https://api.coingecko.com/api/v3/simple/price?ids=ethereum&vs_currencies=ars";

  int coin_id;
  char coin_name[32];
  double value_in_ars;

  while (1)
  {
    printf("Ingrese la criptomoneda:\n 1. Bitcoin\n 2. Monero\n 3. Ethereum\n");
    scanf("%d", &coin_id);

    switch (coin_id)
    {
    case 1:
      if (get_value(url_bitcoin))
      {
        return (1);
      }
      value_in_ars = read_output("bitcoin"); // Valor en $ARS de un bitcoin
      strcpy(coin_name, "BTC");
      break;

    case 2:
      if (get_value(url_monero))
      {
        return (1);
      }
      value_in_ars = read_output("monero"); // Valor en $ARS de un monero
      strcpy(coin_name, "XMR");
      break;

    case 3:
      if (get_value(url_ethereum))
      {
        return (1);
      }
      value_in_ars = read_output("ethereum"); // Valor en $ARS de un ether
      strcpy(coin_name, "ETH");
      break;

    default:
      printf("El valor ingresado no corresponde con ninguna criptomoneda\n");
      break;
    }

    if (value_in_ars == 0)
    {
      return 1;
    }

    printf("Valor de 1 %s en $ARS = %1.0f\n", coin_name, value_in_ars);
  }

  return 0;
}