# Freeze P12-H3d3-D1 — FRAM physical read/write qualification

## Statut

**GELÉ — validation matérielle réelle**

Baseline d'entrée :
- P12-H3d3-C gelé : `8e8f1d21d0bf3ee3874842be92919854f13ac10c`
- backend FRAM brut ajouté : `e3f8d825b7faf7c48599707c73bbfa177eec88b4`
- backend intégré au build STM32 : `53c7c6c9244ed36ce7e4b70660700c237b8220ea`
- qualification physique réversible : `30431b232b7d08566ae2edd803d1b6786114e0fb`
- sondes de résultats stabilisées en uint32_t : `d0a850b6f7be86e1ce1cbde529b54a5572fcc92d`

Cette tranche qualifie les primitives physiques READ/WRITE du MB85RS2MTA sous le contrat `TransactionalImagePhysicalStorage`. Elle ne qualifie pas encore le moteur transactionnel H3d2 sur le média réel.

## 1. Backend physique

Le backend STM32 `stm32_fram_storage` expose :
- `read(context, offset, buffer, size)` ;
- `write(context, offset, buffer, size)` ;
- un adaptateur `TransactionalImagePhysicalStorage`.

Propriétés de D1 :
- capacité bornée : 262144 octets ;
- contrôle offset + taille avant accès ;
- adresse FRAM sur 24 bits, MSB d'abord ;
- SPI1 et brochage déjà qualifiés en H3d3-C ;
- fréquence conservée à 2,5 MHz ;
- opérations de taille zéro acceptées sans accès physique ;
- timeout HAL borné ;
- aucun retry automatique ;
- CS remis à l'état haut après transaction.

## 2. Séquence d'écriture

Commandes utilisées :
- WREN `0x06` ;
- WRDI `0x04` ;
- RDSR `0x05` ;
- READ `0x03` ;
- WRITE `0x02`.

Avant WRITE :
1. WREN ;
2. RDSR ;
3. vérification de WEL, bit `0x02` ;
4. WRITE uniquement si WEL est observé.

Après la tentative destructive, WRDI est envoyé afin de fermer explicitement l'autorisation d'écriture.

D1 ne modifie pas le status register via WRSR et ne modifie donc volontairement ni BP0/BP1 ni WPEN.

## 3. Qualification physique réversible

Zone de test :
- offset `0x1F000` ;
- taille : 8 octets ;
- zone choisie hors des zones utilisées par le profil de qualification H3d2 gelé.

Motif de test :
```text
54 52 32 D1 A5 5A 3C C3
```

Séquence exécutée au démarrage du binaire de qualification :
1. READ des 8 octets originaux ;
2. sauvegarde RAM ;
3. WRITE du motif de test ;
4. READ de vérification ;
5. comparaison exacte ;
6. WRITE des octets originaux ;
7. READ de vérification de restauration ;
8. comparaison exacte avec la sauvegarde.

Les étapes destructives suivantes ne sont pas exécutées si une étape préalable nécessaire échoue.

## 4. Résultats matériels

Après programmation et reset :
- LD1 / PC7 continue de clignoter normalement ;
- le runtime atteint la boucle principale.

Sondes finales GDB :
```text
tr2_fram_d1_init_result           = 0x0
tr2_fram_d1_backup_read_result    = 0x0
tr2_fram_d1_write_result          = 0x0
tr2_fram_d1_verify_read_result    = 0x0
tr2_fram_d1_restore_result        = 0x0
tr2_fram_d1_restore_verify_result = 0x0
```

Comparaisons :
```text
tr2_fram_d1_pattern_matches = 1
tr2_fram_d1_restore_matches = 1
```

Contenu observé :
```text
original = 00 00 00 00 00 00 00 00
readback = 54 52 32 D1 A5 5A 3C C3
restored = 00 00 00 00 00 00 00 00
```

La restauration est donc démontrée : le contenu final de la zone test est identique au contenu sauvegardé avant l'écriture de qualification.

## 5. Anomalie instrumentale GDB levée

Une première sonde de type `Tr2Result` était difficile à interpréter avec l'ELF courant, GDB signalant l'absence d'information de type exploitable.

La qualification finale expose les résultats en `volatile uint32_t` et les lit explicitement avec :
```gdb
p/x (unsigned int)<sonde>
```

Les six résultats sont alors observés sans ambiguïté à `0x0`.

Cette correction concerne uniquement l'instrumentation de qualification ; elle ne modifie pas le protocole FRAM ni H3d2.

## 6. Ce que D1 valide

D1 valide physiquement :
- le backend STM32 vers MB85RS2MTA ;
- READ réel ;
- WREN et observation WEL ;
- WRITE réel ;
- WRDI dans le chemin d'écriture ;
- adressage de la zone test ;
- bornage logiciel ;
- écriture puis relecture exacte ;
- restauration puis relecture exacte ;
- compatibilité du backend avec la forme de `TransactionalImagePhysicalStorage`.

## 7. Limites du gel

D1 ne valide pas encore :
- l'ensemble des offsets de la FRAM ;
- un transfert physique de 51 818 octets ;
- les deux images et superblocs H3d2 sur la FRAM ;
- formatage H3d2 réel ;
- commit transactionnel réel ;
- recovery réel ;
- coupure d'alimentation réelle ;
- performance/durée d'un commit complet ;
- endurance/rétention ;
- fréquence SPI supérieure à 2,5 MHz ;
- DMA ou optimisation ;
- politique finale d'allocation SRAM du buffer candidat.

## 8. Conclusion

P12-H3d3-D1 est validée et gelée pour les primitives physiques FRAM READ/WRITE.

La prochaine sous-tranche peut connecter ce backend au moteur transactionnel H3d2 gelé et qualifier progressivement la géométrie réelle, le format/recovery puis le commit, sans réimplémenter ni modifier les invariants transactionnels H3d2.
