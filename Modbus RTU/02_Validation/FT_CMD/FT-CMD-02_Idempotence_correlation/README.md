# FT-CMD-02 — Idempotence et corrélation

## 1. Objet

FT-CMD-02 valide les règles transactionnelles V1 qui permettent de distinguer une nouvelle commande d'un rejeu Modbus et de corréler sans ambiguïté une réponse du Bloc 5 avec la requête émise.

## 2. Périmètre actif

La sous-famille couvre :
- non-réexécution d'un `transaction_id` déjà traité, sous réserve d'une observabilité suffisante ;
- réutilisation du résultat précédent lors d'un rejeu ;
- traitement d'un nouvel identifiant comme nouvelle transaction lorsque les autres conditions de soumission sont réunies ;
- exposition/corrélation nominale via `cmd_active_transaction_id` ;
- traçage explicite des limites V1 relatives à la mémoire d'idempotence.

## 3. Hors périmètre

Sont exclus :
- validité exacte du domaine de `transaction_id` : dette FT-CMD-01 / V1 non définie ;
- front montant et auto-clear de `submit` : FT-CMD-01 ;
- états finaux, historique et sémantique détaillée des résultats : FT-CMD-03 ;
- comportement d'une commande concurrente pendant une commande active : FT-CMD-04 ;
- effets métier des commandes : FT-CMD-05 à FT-CMD-07 et FT-INT ;
- robustesse hostile au-delà de l'oracle transactionnel V1 minimal : FT-RBT.

## 4. Cas actifs

- `TT-CMD-B05-100` — rejeu immédiat du même `transaction_id` : résultat précédent réutilisé ;
- `TT-CMD-B05-101` — preuve conditionnelle de non-réexécution du même `transaction_id` ;
- `TT-CMD-B05-102` — même code avec nouvel `transaction_id` : nouvelle transaction ;
- `TT-CMD-B05-103` — corrélation nominale de la réponse par `cmd_active_transaction_id`.

## 5. Dettes normatives tracées

- profondeur/durée de conservation des `transaction_id` déjà traités : `NOT_DEFINED` ;
- politique après reboot : déléguée à FT-PER et non définie ici ;
- réutilisation d'un ancien `transaction_id` avec un contenu de requête différent : aucun oracle détaillé ajouté tant que la V1 ne fixe pas explicitement la priorité entre idempotence et incohérence de requête.

## 6. Artefacts

- `source/FT-CMD-02_source.md` ;
- `detaille/FT-CMD-02_detaille.md` ;
- `detaille/FT-CMD-02_matrice_couverture.csv`.

## 7. Statut

Sous-famille reconstruite selon le cadrage validé. Merge interdit avant audit croisé et validation explicite.
