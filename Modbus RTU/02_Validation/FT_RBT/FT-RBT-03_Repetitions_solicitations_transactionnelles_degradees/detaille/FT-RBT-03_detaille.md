# FT-RBT-03 — Cas détaillés et dettes non instanciées

## 1. Aucun test autonome ajouté

Après consolidation des règles V1 et des familles déjà gelées, FT-RBT-03 ne contient aucun nouveau test exécutable autonome.

Cette décision est normative : les comportements déterministes disponibles sont déjà possédés par FT-CMD-02, FT-CMD-04 et FT-RBT-02. Les dimensions restantes de répétition agressive ou de charge ne disposent pas de seuils V1 permettant un verdict PASS/FAIL non arbitraire.

## 2. Scénarios non dupliqués

### Rejeu immédiat d'un identifiant déjà traité

Utiliser les tests FT-CMD-02. Ne pas créer un clone FT-RBT sous prétexte que le rejeu est une répétition protocolaire.

### Perte de réponse puis retransmission

Utiliser FT-RBT-02, qui possède la perturbation de communication et compose l'oracle d'idempotence FT-CMD.

### Nouvelle commande pendant commande active

Utiliser FT-CMD-04. Le refus de concurrence n'est pas un nouvel oracle FT-RBT.

## 3. Scénarios non instanciés faute d'oracle V1

### Rafale du même identifiant

Aucun test « rejouer N fois » n'est créé. La V1 ne définit ni N, ni cadence, ni durée minimale de rétention de l'historique d'idempotence.

### Sollicitations à haute cadence

Aucun test de requêtes toutes les X ms, de débit minimal ou de temps de réponse maximal n'est créé. Ces valeurs ne sont pas spécifiées.

### Accumulation / file d'attente

Aucun test de profondeur de queue n'est créé. Le fait qu'une seule commande puisse être active ne permet pas d'inférer l'existence ou l'absence d'une file interne au-delà du comportement de refus déjà normé.

### Même ID, payload différent

Aucun verdict n'est ajouté. La dette reste celle de FT-CMD-02.

## 4. Usage futur

Ces scénarios peuvent devenir des tests d'endurance, de caractérisation ou d'acceptation non normative si un cahier d'essai fixe explicitement ses propres seuils. Ils ne doivent alors pas être présentés comme validation de conformité V1.
