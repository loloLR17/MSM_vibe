# P12-H3h-C2b — Qualification physique SDMMC2 1-bit / 10 MHz en écriture

## Objet

Consigner la première mesure physique destructive d'écriture brute microSD sur le
chemin SDMMC2 1-bit / 10 MHz. La carte utilisée est explicitement un média de test
sacrifiable.

Cette tranche mesure une transaction de 128 blocs logiques contigus, attend le
retour réel de la carte à l'état TRANSFER, puis relit et compare intégralement la
zone écrite.

## Baseline firmware testée

- Commit : `dc98ec39a17173aed1e35955221dd0d2588e139b`
- Message : `Firmware: probe SDMMC2 1-bit write throughput`
- SDMMC2 : 1 bit, ClockDiv=8, chemin 10 MHz
- Bloc initial : 2048
- Nombre de blocs : 128
- Taille bloc : 512 octets
- Volume : 65 536 octets
- Pattern : déterministe
- Filesystem / format / erase : aucun

## Résultat physique

| Variable | Valeur |
|---|---:|
| `tr2_sdmmc2_stage` | 14 |
| `tr2_sdmmc2_write_status` | 0 / HAL_OK |
| `tr2_sdmmc2_write_elapsed_ms` | 298 ms |
| `tr2_sdmmc2_write_bytes` | 65 536 B |
| `tr2_sdmmc2_write_throughput_bps` | 219 919 B/s |
| `tr2_sdmmc2_ready_state` | 4 |
| `tr2_sdmmc2_verify_read_status` | 0 / HAL_OK |
| `tr2_sdmmc2_verify_mismatch_count` | 0 |
| `tr2_sdmmc2_verify_first_mismatch` | 0xffffffff |
| `tr2_sdmmc2_error_code` | 0x0 |

Le stage 14 est le succès complet du probe : écriture acceptée, carte revenue à
TRANSFER, relecture réussie et comparaison sans mismatch.

Le débit mesuré inclut le retour à TRANSFER :

```text
65 536 * 1000 / 298 = 219 919 B/s
```

## Confrontation au contrat bulk

Débit logique gelé P8-B :

```text
426 672 B/s
```

Rapport mesuré / requis :

```text
219 919 / 426 672 = 0,5154
```

Cette transaction fournit donc environ 51,5 % du débit logique requis. Le déficit
est d'environ 206 753 B/s.

## Conclusion bornée

**C2b valide l'intégrité de l'écriture/relecture brute, mais ne valide pas la
performance requise pour le bulk TR2.**

Cette mesure unique de 64 KiB ne permet pas encore de distinguer un débit soutenu
insuffisant d'une latence ponctuelle ou périodique de programmation/gestion interne
de la carte. Elle ne justifie donc ni le rejet de la microSD ni un changement
immédiat de largeur de bus.

## Frontière suivante — H3h-C2c

Mesurer plusieurs écritures contiguës de 64 KiB en conservant strictement le même
chemin 1-bit / 10 MHz. Relever les durées par chunk et la durée globale, attendre
TRANSFER après chaque écriture, puis vérifier par relecture.

C2c doit permettre d'observer la dispersion et le débit soutenu avant toute
modification de fréquence ou réouverture du mode 4-bit.
