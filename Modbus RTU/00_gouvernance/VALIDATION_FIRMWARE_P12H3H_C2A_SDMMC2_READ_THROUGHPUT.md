# P12-H3h-C2a — Qualification physique SDMMC2 1-bit / 10 MHz en lecture

## Objet

Consigner la mesure physique H3h-C2a du chemin microSD SDMMC2 sur NUCLEO-U575ZI-Q,
sans filesystem et sans aucune écriture sur la carte.

Cette tranche est une caractérisation de lecture brute. Elle ne qualifie pas encore
le débit d'écriture requis par le CampaignDataStore.

## Baseline firmware testée

- Commit firmware : `1ee7c03bc34139f48a01d964a26d7e581c515b76`
- Message : `Firmware: measure SDMMC2 1-bit read throughput`
- Interface : SDMMC2
- Largeur : 1 bit
- ClockDiv : 8, chemin précédemment qualifié à 10 MHz
- Mode : lecture brute HAL, sans filesystem
- Bloc initial : 0
- Nombre de blocs : 128
- Taille bloc logique : 512 octets
- Volume total : 65 536 octets
- Écriture / erase / format : aucun

## Résultat physique

Valeurs relevées sous GDB après exécution du probe :

| Variable | Valeur |
|---|---:|
| `tr2_sdmmc2_stage` | 12 |
| `tr2_sdmmc2_error_code` | 0x0 |
| `tr2_sdmmc2_read_status` | 0 |
| `tr2_sdmmc2_read_nonzero` | 1 |
| `tr2_sdmmc2_read_elapsed_ms` | 53 ms |
| `tr2_sdmmc2_read_bytes` | 65 536 B |
| `tr2_sdmmc2_read_throughput_bps` | 1 236 528 B/s |

Le stage 12 signifie que la transaction multi-bloc a réussi et que le buffer contient
au moins un octet non nul. Le statut HAL vaut HAL_OK et aucun bit d'erreur SDMMC2
n'est remonté.

Le débit est calculé par le firmware à partir de la mesure milliseconde :

```text
65 536 * 1000 / 53 = 1 236 528 B/s
```

## Confrontation au contrat bulk P8-B

Le débit logique bulk gelé vaut :

```text
26 667 échantillons/s * 16 octets = 426 672 B/s
```

La mesure de lecture H3h-C2a représente donc :

```text
1 236 528 / 426 672 = 2,898
```

soit environ 2,90 fois le débit logique et une marge relative de lecture d'environ
+190 % par rapport à ce flux.

## Conclusion bornée

**H3h-C2a est validé pour la caractérisation physique en lecture brute du chemin
SDMMC2 1-bit / 10 MHz.**

La mesure montre que ce chemin n'est pas limité, en lecture brute sur cette
transaction de 64 KiB, au voisinage du débit logique bulk de 426 672 B/s.

Cette conclusion ne doit pas être étendue à l'écriture :

- aucune écriture microSD n'a été exécutée ;
- aucune latence longue ou variabilité d'écriture de la carte n'a été mesurée ;
- aucun comportement soutenu CampaignDataStore n'est qualifié ;
- aucun filesystem n'est introduit ;
- le mode 4-bit reste hors du chemin critique et n'est pas rouvert par cette tranche.

## Frontière suivante — H3h-C2b

La prochaine qualification physique doit mesurer explicitement l'écriture brute
multi-bloc sur une zone de média de test autorisée à être détruite.

Avant toute exécution H3h-C2b, la zone/carte de test et le caractère destructif de
l'opération doivent être explicitement établis. H3h-C2b devra mesurer au minimum :

- volume effectivement écrit ;
- durée de transaction ;
- débit brut d'écriture ;
- statut HAL et code d'erreur ;
- vérification de contenu par relecture ;
- comparaison au débit logique gelé de 426 672 B/s.

Le backend physique CampaignDataStore reste hors de H3h-C2b.
