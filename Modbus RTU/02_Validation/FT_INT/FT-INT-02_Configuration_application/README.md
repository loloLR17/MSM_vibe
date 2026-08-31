# FT-INT-02 — Configuration / application

## 1. Objet

FT-INT-02 valide les relations inter-blocs explicitement imposées entre le Bloc 4 et le Bloc 5 lors de l'application d'une configuration préparée.

## 2. Périmètre actif

La sous-famille couvre :
- application effective d'une configuration préparée après commande B5 réussie ;
- mise à jour de l'identité de configuration active ;
- transition `config_state` vers `ACTIF` après succès ;
- cohérence de l'image active 4E avec la configuration effectivement appliquée.

## 3. Hors périmètre

Sont exclus :
- absence d'effet immédiat d'une écriture dans la zone préparée : FT-BLK-04 ;
- calcul CRC préparé, algorithme, sérialisation et vecteur normatif : FT-BLK-04 ;
- domaines et valeurs invalides : FT-LIM ;
- droits d'accès : FT-ACC ;
- acceptation/refus, codes résultat, idempotence et moteur transactionnel B5 : FT-CMD ;
- effet de la configuration active sur la supervision B3 : FT-INT-03 ;
- persistance après reboot ou coupure : FT-PER.

## 4. Cas actifs

- `TT-INT-B04B05-001` — application effective de la configuration préparée ;
- `TT-INT-B04B05-002` — mise à jour de l'identité active ;
- `TT-INT-B04B05-003` — transition d'état vers `ACTIF` ;
- `TT-INT-B04B05-004` — cohérence de l'image active 4E après application.

## 5. Relations non actives

- échec réel d'application conduisant à `ERREUR_APPLICATION` : `CONDITIONAL` ;
- refus pour configuration non `VALIDE`, CRC incorrect ou configuration incomplète : `DELEGATED` vers FT-CMD.

## 6. Artefacts

- `source/FT-INT-02_source.md` ;
- `detaille/FT-INT-02_detaille.md` ;
- `detaille/FT-INT-02_matrice_couverture.csv`.

## 7. Statut

Sous-famille reconstruite selon le cadrage validé. Gel interdit avant audit croisé et validation explicite.
