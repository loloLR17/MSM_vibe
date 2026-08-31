# FT-INT-04 — Acquisition et campagnes

## 1. Objet

FT-INT-04 valide les relations fonctionnelles inter-blocs associées au démarrage et à l'arrêt de l'acquisition, à l'ouverture et à la clôture des campagnes, à leur base temporelle et au non-effacement des campagnes par la RAZ statistiques.

Blocs concernés :
- Bloc 0 — identité capteur, pour le contrôle transversal de non-effacement par RAZ statistiques ;
- Bloc 1 — état système ;
- Bloc 2 — temps ;
- Bloc 4 — configuration, pour le contrôle transversal de non-effacement par RAZ statistiques ;
- Bloc 5 — commandes ;
- Bloc 6 — inventaire campagnes.

## 2. Doctrine

FT-INT-04 ne revalide pas le moteur transactionnel du Bloc 5. Un résultat B5 terminé avec succès constitue le stimulus à partir duquel sont observés les effets dans les autres blocs.

Statuts utilisés :
- **COVERED** : oracle V1 déterministe ;
- **CONDITIONAL** : oracle normatif mais dépendant d'un contexte ou d'une instrumentation maîtrisée ;
- **DELEGATED** : propriété portée par une autre famille ;
- **TRACE_ONLY** : relation utile à tracer mais insuffisante seule pour PASS/FAIL ;
- **NOT_DEFINED** : relation plausible mais non normée en V1.

## 3. Périmètre actif

FT-INT-04 couvre :
- succès START B5 → acquisition B1 active ;
- succès START B5 → ouverture d'une nouvelle campagne B6 ;
- cohérence de la campagne ouverte avec l'état « en cours » ;
- succès STOP B5 → acquisition B1 arrêtée ;
- STOP → mise en cohérence / clôture de la campagne ouverte ;
- utilisation par B6 de la base temporelle B2 ;
- conservation des campagnes, de l'identité et de la configuration après RAZ statistiques B5.

## 4. Limites normatives conservées

FT-INT-04 n'impose pas :
- l'égalité `B1.active_campaign_id == B6.campaign_id` de la campagne courante : **NOT_DEFINED** ;
- un incrément immédiatement observable et strict `total_campaign_count + 1` après START : **NOT_DEFINED** ;
- une formule universelle exacte `duration_s = end_timestamp - start_timestamp` : déjà identifiée comme insuffisamment normée par FT-BLK-05 ;
- une égalité bit-à-bit entre une lecture B2 et les timestamps B6 obtenus lors de lectures Modbus séparées.

## 5. Délégations

Restent hors FT-INT-04 :
- mécanisme `submit`, `transaction_id`, idempotence et statuts du moteur B5 : **FT-CMD** ;
- refus START/STOP et codes résultat associés (`21`, `22`, absence SD, défaut critique, etc.) : **FT-CMD** ;
- invariants internes de l'entrée B6 (`campaign_id != 0`, `end_timestamp = 0` si campagne en cours, sélection) : **FT-BLK-05** ;
- permissions et domaines : **FT-ACC / FT-LIM** ;
- snapshot, atomicité et cohérence multi-registres : **FT-STR** ;
- persistance après redémarrage : **FT-PER**.

## 6. Cas de test actifs

- `TT-INT-B01B05-001` — Activation effective de l'acquisition
- `TT-INT-B05B06-001` — Ouverture d'une nouvelle campagne au démarrage
- `TT-INT-B05B06-002` — Cohérence de la campagne en cours
- `TT-INT-B01B05B06-001` — Arrêt acquisition et clôture de campagne
- `TT-INT-B02B06-001` — Base temporelle des campagnes
- `TT-INT-B00B04B05B06-001` — Conservation identité/configuration/campagnes après RAZ statistiques

## 7. Artefacts

- `source/FT-INT-04_source.md`
- `detaille/FT-INT-04_detaille.md`
- `detaille/FT-INT-04_matrice_couverture.csv`

## 8. Statut

Sous-famille reconstruite selon le cadrage validé. Gel interdit avant audit croisé et validation explicite.