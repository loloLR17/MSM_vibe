# P12-H3g-B — Qualification physique SPI3 / WHO_AM_I IIS3DWB

## Résultat

Qualification physique acquise sur NUCLEO-U575ZI-Q + STEVAL-MKI208V1K.

Câblage qualifié :
- PC9 / D44 -> CS
- PC10 / D45 -> SCL / SPI3_SCK
- PC11 / D46 <- SDO / SPI3_MISO
- PC12 / D47 -> SDA / SPI3_MOSI
- VDD = 3,3 V
- VDDIO = 3,3 V
- masse commune

Une inversion initiale SDA/SDO a produit 0xFF avec HAL_OK. Après correction du câblage, le test physique donne :
- tr2_iis3dwb_spi_init_ok = 1
- tr2_iis3dwb_whoami_status = HAL_OK (0)
- tr2_iis3dwb_whoami = 0x7B
- tr2_iis3dwb_whoami_matches = 1

Conclusion : chemin SPI3 STM32U575 -> STEVAL -> IIS3DWB physiquement démontré.

La FRAM SPI1 et sa baseline qualifiée ne sont pas modifiées.

Suite : H3g-C, première acquisition physique X/Y/Z contrôlée.
