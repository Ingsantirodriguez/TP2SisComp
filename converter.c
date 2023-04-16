#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "lib/cJSON.h"

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

double read_output()
{
  double ars_value = 0;
  // Open the file for reading
  FILE *fp = fopen("output.json", "r");
  if (fp == NULL)
  {
    fprintf(stderr, "Error opening file\n");
    return 1;
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
    return 1;
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
    return 1;
  }

  // Extract the "ars" value from the JSON object
  cJSON *ars_obj = cJSON_GetObjectItemCaseSensitive(cJSON_GetObjectItem(root, "bitcoin"), "ars");
  if (cJSON_IsNumber(ars_obj))
  {
    ars_value = ars_obj->valuedouble;
  }
  else
  {
    cJSON *ars_obj = cJSON_GetObjectItemCaseSensitive(cJSON_GetObjectItem(root, "monero"), "ars");
    if (cJSON_IsNumber(ars_obj))
    {
      ars_value = ars_obj->valuedouble;
    }
    else
    {
      fprintf(stderr, "Error getting \"ars\" value from JSON\n");
      return 0;
    }
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

  if (get_value(url_bitcoin))
  {
    return (1);
  }
  double bitcoin_in_ars = read_output(); // Valor en $ARS de un bitcoin
  if (get_value(url_monero))
  {
    return (1);
  }
  double monero_in_ars = read_output(); // Valor en $ARS de un monero

  if (bitcoin_in_ars == 0 || monero_in_ars == 0)
  {
    return 1;
  }

  printf("Valor de 1 BTC en $ARS = %1.0f\nValor de 1 XMR en $ARS = %1.0f\n", bitcoin_in_ars, monero_in_ars);
  return 0;
}