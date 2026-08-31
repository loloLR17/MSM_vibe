# FT-RBT-03 — Répétitions et sollicitations transactionnelles dégradées

## 1. Objet

FT-RBT-03 examine les répétitions et sollicitations dégradées autour du moteur transactionnel B5 qui ne sont pas déjà entièrement possédées par FT-CMD.

## 2. Conclusion de périmètre

La V1 fournit un oracle déterministe pour le rejeu d'une transaction déjà traitée, déjà couvert par FT-CMD-02 et composé avec perte de réponse dans FT-RBT-02. Elle fournit aussi un oracle de concurrence pour une nouvelle commande soumise pendant une commande active, déjà couvert par FT-CMD-04.

Aucune règle supplémentaire ne définit une fréquence maximale de répétition, une profondeur de rafale, une cadence admissible, une file d'attente, un débit transactionnel minimal ou une priorité générique entre sollicitations rapprochées.

FT-RBT-03 ne crée donc aucun nouveau test PASS/FAIL autonome sur ces dimensions.

## 3. Cas tracés

- rejeu du même `transaction_id` déjà traité : `DELEGATED` à FT-CMD-02 ;
- perte de réponse puis rejeu du même `transaction_id` : `DELEGATED` à FT-RBT-02 ;
- nouvelle commande pendant commande active : `DELEGATED` à FT-CMD-04 ;
- répétitions multiples / rafales du même identifiant : `NOT_DEFINED` au-delà du rejeu immédiat déjà normé ;
- cadence maximale / débit minimal / espacement minimal : `NOT_DEFINED` ;
- profondeur de file / accumulation de requêtes : `NOT_DEFINED` ;
- même identifiant avec contenu différent : `DELEGATED` à la dette FT-CMD-02, elle-même `NOT_DEFINED`.

## 4. Doctrine

L'absence de nouveau test n'est pas une lacune de validation : elle matérialise l'absence d'oracle V1 supplémentaire. Aucun comportement de charge ou de stress ne doit être transformé en exigence normative sans évolution de spécification.

## 5. Artefacts

- `source/FT-RBT-03_source.md` ;
- `detaille/FT-RBT-03_detaille.md` ;
- `detaille/FT-RBT-03_matrice_couverture.csv`.

## 6. Statut

Sous-famille reconstruite pour revue. Gel interdit avant validation explicite.
