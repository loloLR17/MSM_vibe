# FT-LIM-05 — Vue d’ensemble des instances

## Couverture

FT-LIM-05 contient 12 lignes d’instances décrivant 10 cas génériques. La majorité est conditionnelle car la valeur N provient de l’inventaire réel et ne doit pas être fabriquée en écrivant un registre RO.

## Points de preuve principaux

- N=0 : index 0 est fonctionnellement invalide mais accepté Modbus.
- N>0 : index 0 et N-1 sont valides.
- N>2 : un index intérieur est vérifiable.
- index N et 65535 hors plage : acceptés Modbus, `selected_campaign_valid=0`.
- sélection valide : `campaign_id!=0`.
- sélection invalide : aucune hypothèse sur le contenu exact des métadonnées.
- deux sélections successives : pas de mélange sémantique entre campagnes.

## Particularités

`TT-LIM-05-C05-002` documente explicitement que pour N=0, le cas « première valeur hors plage index=N » est identique au test inventaire vide et ne doit pas créer un faux doublon de couverture.

Les enums RO du Bloc 6 sont tracés dans la matrice mais différés hors FT-LIM-05.
