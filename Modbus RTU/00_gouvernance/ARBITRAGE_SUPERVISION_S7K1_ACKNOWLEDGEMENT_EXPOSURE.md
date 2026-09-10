# S7-K1 — Fermeture de l’exposition Web de l’acquittement global

## Objet

Cette tranche corrige un écart détecté pendant l’audit transversal S7-K entre l’IHM Web réellement exposée et les invariants gelés en S7-I.

## Constat

Le gel S7-I impose qu’aucun acquittement global ne soit exposé par l’IHM. La page opérateur ne proposait plus d’action « acquitter tout », mais l’endpoint HTTP `POST /api/v1/devices/{deviceId}/commands` acceptait encore un `AcknowledgeFault` avec `acknowledgeAll=true`.

L’API HTTP fait partie de la surface Web S7. Ce comportement constituait donc une exposition résiduelle non conforme au gel UX.

## Décision

À partir de S7-K1 :

- l’API Web S7 n’accepte que l’acquittement ciblé ;
- `AcknowledgeFault` exige `acknowledgeAll=false` et un `faultCode` explicite ;
- `acknowledgeAll=true` est rejeté par l’API avec une requête invalide avant appel du command sink ;
- aucune action globale n’est donc accessible ni depuis la page Web ni directement via l’API S7.

## Frontières conservées

Cette correction ne modifie pas :

- la spécification Modbus V1 ;
- le mapping B5 firmware ;
- les capacités internes du runtime de supervision ;
- le moteur transactionnel B5 ;
- la politique `Ambiguous` ;
- les règles de persistance/recovery ;
- B6 ;
- les autres commandes Web déjà validées.

La capacité interne du runtime à représenter le paramètre B5 correspondant n’est pas supprimée : seule son exposition Web S7 est fermée.

## Test

Le test HTTP existant est resserré pour vérifier :

- l’acceptation d’un acquittement ciblé valide ;
- le rejet de l’absence de cible ;
- le rejet de `acknowledgeAll=true` ;
- l’absence d’appel au command sink pour la requête globale rejetée.

## Statut

Cette tranche doit être validée localement par build et tests avant poursuite du gel final S7-K.
