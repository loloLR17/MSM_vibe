# FT-RBT-03 — Exigences source normalisées

## 1. Références normatives

- `01_Specification_source/bloc5.md` V1 ;
- `02_Validation/FT_CMD/FT-CMD-02_Idempotence_correlation/` ;
- `02_Validation/FT_CMD/FT-CMD-04_Concurrence/` ;
- `02_Validation/FT_RBT/FT-RBT-02_Perte_reponse_retransmission_transactionnelle/` ;
- plan maître de validation Modbus TR2.

## 2. Exigences et dettes retenues

### RBT03-R01 — Rejeu immédiat d'une transaction traitée
- Classification : `DELEGATED`.
- Propriétaire : FT-CMD-02.
- Motif : l'idempotence et la réutilisation du résultat sont déjà couvertes directement.

### RBT03-R02 — Perte de réponse suivie d'un rejeu
- Classification : `DELEGATED`.
- Propriétaire : FT-RBT-02.
- Motif : c'est précisément la composition robuste déjà créée.

### RBT03-R03 — Nouvelle commande pendant commande active
- Classification : `DELEGATED`.
- Propriétaire : FT-CMD-04.
- Motif : la V1 définit déjà le refus transactionnel de concurrence ; FT-RBT-03 ne duplique pas cet oracle.

### RBT03-R04 — Rafale de répétitions du même transaction_id
- Classification : `NOT_DEFINED` au-delà du rejeu immédiat.
- Justification : la V1 garantit l'idempotence d'un identifiant déjà traité mais ne fixe ni nombre de répétitions, ni cadence, ni fenêtre temporelle de conservation permettant un test de rafale borné.
- Interdiction : ne pas choisir arbitrairement N répétitions ou une durée d'essai et en faire une exigence V1.

### RBT03-R05 — Cadence maximale / espacement minimal / débit transactionnel
- Classification : `NOT_DEFINED`.
- Justification : aucun seuil de fréquence, temps inter-requête, débit minimal ou latence maximale n'est spécifié.

### RBT03-R06 — File d'attente / accumulation de requêtes
- Classification : `NOT_DEFINED`.
- Justification : la V1 définit une seule commande active et le refus d'une nouvelle commande concurrente, mais ne définit aucune profondeur de queue ni politique d'accumulation.

### RBT03-R07 — Même transaction_id avec contenu différent
- Classification : `DELEGATED` vers la dette FT-CMD-02 (`NOT_DEFINED`).
- Justification : la priorité entre identité transactionnelle et différence de payload n'est pas définie.

## 3. Anti-fabrication

FT-RBT-03 ne doit pas :
- transformer un test de stress en exigence fonctionnelle ;
- inventer un nombre minimal de répétitions supportées ;
- inventer une fréquence de requêtes admissible ;
- supposer une file d'attente interne ;
- imposer une durée de mémorisation de l'idempotence ;
- redéfinir la concurrence déjà couverte par FT-CMD-04.

## 4. Conclusion normative

Aucun cas autonome `COVERED` ou `CONDITIONAL` supplémentaire n'est justifié dans FT-RBT-03 avec la V1 actuelle. La sous-famille est néanmoins conservée pour rendre explicites les frontières et les dettes de robustesse.
