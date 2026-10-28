# MSM TR2 — R0-MÉTIER — Matrice initiale de réutilisation

**Statut : AUDIT DOCUMENTAIRE INITIAL — NON GELÉ (29/10/2026)**. Le statut `CONSERVER` n'est pas une qualification industrielle. Référence : [cahier métier](R0_METIER_20261029_CAHIER_DES_CHARGES.md).

| Fonction | Observation du dépôt / des rapports | Décision R0 proposée | Priorité |
|---|---|---|---|
| STM32U575 + IIS3DWB | Acquisition physique FIFO bornée démontrée ; ±2 g dans dernière image physiquement qualifiée ; ±8 g logiciel en mission Codex non qualifiée physique | **CONSERVER** capteur et CPU, vérifier ±8 g réel, sensibilité/bruit/saturation | P1 |
| Acquisition FIFO | Flux XYZ continu consommé, mais pas encore archivé sur 8 h | **CONSERVER/ADAPTER** l'unique pipeline FIFO vers archive continue et compteurs de perte | P1 |
| RMS/crêtes LIVE | `live_metrology_sample` saute le calcul hors fenêtres (4096/26667≈154ms, period2000ms) | **ADAPTER** la couverture des indicateurs, sans bloquer l'archive | P1 |
| B3 et alarmes | [B3](../01_Specification_source/bloc3.md) et [LIVE R0](../01_Specification_source/LIVE_R0/PROFIL_LIVE.md) calculent l'accélération absolue sans DC retiré | **RÉVISER DE FAÇON VERSIONNÉE** les indicateurs métier ; ne pas réécrire les anciennes valeurs | P1 |
| AC1 expérimental | Passe-haut 2 Hz et indicateurs séparés, checkpoint Codex `IN_PROGRESS`, tests logiciels | **CONSERVER comme banc**, pas comme norme ni comme contenu de l'archive | P2 |
| Fréquence et traitement | B4 n'accepte que 26667 Hz ; objectif d'archive 2000 Hz ; pas de filtre décimateur qualifié | **ADAPTER** filtre anti-repliement, cadence exacte, timestamps, mémoire/CPU | P1 |
| Bulk logique | Blocs/CRC, metadata, checkpoint, recovery, `CampaignDataStore` existent, tests hôte ; record historiquement16B/sample | **CONSERVER/ADAPTER** codec, format, budgets et version ; pas de réécriture générale | P1 |
| SDMMC matériel | C2c 512Kio à1,20Mo/s relus ; M017 overflow après0,402315s ; autres CRC/NoMedia sur SDMMC1 | **REMPLACER le chemin matériel non qualifié** ou garder comme archive expérimentale ; pas de reprise aveugle des mêmes essais | P1 bloquant |
| RAM et temps réel | Checkpoint AC1 indique BSS ≈659Kio? **659176 octets**, restant et stack/heap/IRQ non qualifiés pour buffer SD + DSP | **AUDITER** les maxima et le backpressure avant intégration | P1 |
| FRAM/config/identité | Autorités durables et prepared/active déjà construites | **CONSERVER**, évolution de schéma minimale/versionnée | P1 préservation |
| Modbus RTU/B5 | Communication, commandes transactionnelles et STOP physique observés (~40 s) | **CONSERVER**, revoir arrêt/flush de campagne et statut sans revendiquer sécurité machine | P1/P2 |
| Horloge/synchronisation | Bloc2 + horodatage disponibles, synchronisation multicapteur non prouvée | **ADAPTER ET QUALIFIER** dérive et corrélation du temps | P1 |
| PC, SQLite, interface | LIVE réel/mesures synthétiques, mais pas d'import de fichier XYZ 8h | **CONSERVER le socle**, ajouter import/contrôle de fichiers et analyse hors ligne | P2 |
| Extraction bulk | Plusieurs centaines de Mo par capteur à récupérer | **DÉFINIR** support et méthode d'extraction dédiée ou amovible | P1 |
| FFT embarquée/UX/alertes évoluées | Non indispensables à la première mesure 8h | **DIFFÉRER**, préserver le code existant | P3 |
| Fixation/qualification bord | Six emplacements, orientations et conditions réelles non qualifiés | **QUALIFIER PLUS TARD** montage, vibrations fortes, comparaison métier | P2 |

## Contradictions et trous avérés

1. **Objectif enregistreur autonome vs trajectoire historique LIVE sans stockage** : la [roadmap A-06](ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md) reporte le stockage en R7, alors que la nouvelle V1 le rend bloquant. **Nouvelle trajectoire distincte**, pas effacement de l'historique.
2. **Continu vs fenêtre partielle** : la FIFO tourne, mais la publication RMS/crêtes n'intègre qu'une fenêtre sur une période ; les échantillons intermédiaires ne forment pas une archive durable.
3. **Vibratoire vs pesanteur** : contrat B3 absolu. Séparer données archivées (DC préservé) et indicateurs dynamiques (DC soustrait).
4. **26667Hz vs 2000Hz enregistrés** : la cible de fréquence ne se satisfait pas d'un changement d'étiquette ; DSP et horodatage doivent être caractérisés.
5. **Stockage physique non qualifié**, malgré infrastructure logique riche : nouveau backend à prouver avant assemblage final.
6. **Importation / récupération de l'archive** : un flux stocké sans moyen raisonnable de récupération ne satisfait pas l'objectif.

## Références factuelles

- [M017 SDMMC 1bit — RUN FAILED](Echanges_Codex/archives/TR2-20261014-M017-SDMMC-1BIT-REALISTIC-QUALIFICATION/RAPPORT.md) ; [SDMMC1 communauté — FAILED](Echanges_Codex/archives/TR2-20261014-P12H3H-SDMMC1-REFERENCE-POC/RAPPORT.md) ; [C2c écriture courte](VALIDATION_FIRMWARE_P12H3H_C2C_SDMMC2_SUSTAINED_WRITE.md).
- [D5-C — stockage logique](VALIDATION_FIRMWARE_P12H3H_D5C_BULK_DATA_STORE.md) ; `../05_Firmware/src/application/live_metrology.c`, `../05_Firmware/platform/stm32/live_fifo_acquisition.c`.
- [Checkpoint AC1](Echanges_Codex/DERNIER_RAPPORT.md) : `IN_PROGRESS`, nouvelle image **non physiquement qualifiée** à la date de ce document.

Cette matrice est un **audit de réutilisation initial**, pas encore une analyse exhaustive des interfaces et du couplage logiciel : les détails seront établis avant toute modification du code.
