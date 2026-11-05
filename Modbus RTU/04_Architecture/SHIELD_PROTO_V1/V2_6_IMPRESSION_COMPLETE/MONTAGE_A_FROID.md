# Montage et ajustement — hors tension

## 1. Trier et préparer

Séparer les séries H8/H10/H12 et les cales0,5/1/2mm. Les appuis ont un profil arrondi4,4×4mm, trou2,4mm ; le côté4mm se place suivant Y, parallèlement au petit dégagement de la bouche SD. Les STEP sont des coordonnées d’assemblage ; ne pas confondre leur orientation avec les STL de plateau.

Charger les neuf écrous captifs par caisse nue, puis suivre le montage V2.5 des Nucleo/D126/bras. **Ajile :6 vis M2×20+12écrous nylon utiles pour deux TR2 ;2vis+4écrous de réserve**, aucun prélèvement pour les modules. G12,25 conservé ; ne pas déformer le shield pour obtenir cette hauteur.

## 2. FRAM et microSD — compression vers le shield

À chaque point : tête M2×30 sous shield → rondelle PETG1mm → NPTH shield → appui/cales rigides PETG → module → rondelle PETG1mm → **deux écrous M2 ordinaires en contre-écrou**. Le filetage passe librement dans les perçages, sans contact voulu avec le cuivre. Les quatre trous Adafruit sont métallisés flottants sur les sources de référence ; les rondelles isolent les têtes/écrous de leur surface et des circuits voisins. Vérifier les vraies révisions, les soudures et les portées.

1. Emboîter le module à la main, à sec, jusqu’à sa **butée plastique naturelle**. Ne jamais utiliser l’écrou ou l’appui comme presse. Conserver les corps entiers des femelles1×10 ; aucune découpe de logement.
2. Présenter deux appuis sans forcer. Choisir parmi H8/H10/H12, puis les cales0,5/1/2. La série couvre8..15,5mm par pas0,5 ; si nécessaire choisir le pas supérieur et **poncer la face supérieure plane de l’appui de0..0,5mm maximum**, sur une cale abrasive, hors banc. Ne pas poncer une cale0,5 fragile ni le circuit.
3. Les deux portées doivent arriver juste au contact, sans basculement et sans fléchir le module. S’il manque encore de la hauteur, ajouter une cale ; si le module se soulève ou le connecteur recule, reprendre le calage. H est une hauteur mécanique obtenue **après emboîtement**, pas un déplacement électrique autorisé.
4. Poser la rondelle supérieure, puis deux écrous. Serrer au contact très léger et tenir le premier pour bloquer le second, sans appliquer le couple au PCB. Vérifier que la tige ne touche aucun composant ou câble. La vis M2×30 unique laisse de la réserve de longueur ; pas besoin de commander une longueur sur mesure.
5. Insérer/retirer la microSD sur toute sa course, porte ouverte. Aucune rondelle ou écrou ne doit gêner la coque ou l’accès. Confirmer les appuis et les têtes sous shield avant fermeture.

La chaîne fabricant donne femelle8,5mm ; un collier mâle standard2,5mm conduit à **H≈11mm, hypothèse de départ uniquement**. Le STEP Click montre un collier2,54mm, donc une référence légèrement différente ; aucune hauteur réelle des mâles Adafruit n’est inventée. Les fenêtres de contact internes Connfly ne sont pas cotées dans la fiche : ni la course8..15,5 ni un calcul de broche ne remplacent l’emboîtement manuel.

## 3. Deux étriers, quatre patins par TR2

L’étrier MKI se fixe aux deux axes(39;20) et(71;54,5), le Click aux axes(91;75) et(94,5;112,5). Fixer chacun par deux M2×30 depuis dessous, rondelles PETG1mm sous les têtes, puis deux écrous par point sur l’épaulementZ34,5. Les pieds de l’étrier portent directement sur le shield, aucun écrou suspendu en guise d’entretoise.

Avant fixation, charger les deux **DIN985M3** dans leurs poches latérales, bague nylon vers le haut ; insérer les patins depuis le dessous et engager les M3×12. L’anti-rotation vient des guides carrés ; les patins avant/arrière MKI sont différents. Le capot d’écrou n’est pas une empreinte pour un écrou M3DIN934 du coffret.

- MKI : patin avant au bord court près de(48;23,4), arrière près de(48;52,5). Ne pas couvrir les rangéesJP1/JP2 ou l’IDC. La géométrie provient du vrai contour Gerber ST, plus étroit que l’ancienne enveloppe provisoire.
- Click : deux patins au bord logique près de(84,2;86,75) et(84,2;96,75). Aucun métal ne traverse la barrière vers GND2/le bornier. Le STEP officiel estv1.01 ; ChatGPT rapporte sur photo le vrai moduleHWREV1.02. Confirmer ses zones nues au montage : la géométrie des composants1.02 n’a pas été assimilée automatiquement à1.01. Les ouvertures observées ne sont pas utilisées comme trous de fixation.
- Régler doucement au contact sur du **PCB nu**, puis reculer environ1/5tour (0,1mm de filetage M3). Aucun effort appuyé sur la carte. Vérifier la retenue en soulevant très légèrement à la main : elle doit limiter le soulèvement avant que les connecteurs se désengagent. Le jeu de poche0,4mm, la tolérance de l’écrou et celle de l’impression s’ajoutent au jeu de réglage ; ne pas considérer0,1mm comme une mesure garantie.
- La coupelle conserve1,5mm dePETG sous la pointe de vis. Aucun boulon ne doit traverser le patin. Ne pas remplacer ces patins par une vis en contact direct avec le PCB. Si un composant ou une soudure occupe la portée, retirer et ajuster uniquement la pièce PETG hors banc ; ne jamais forcer, rogner un composant ou déplacer le PCB commandé.

La plage de hauteur du dessus de module admise par ces retenues estZ24,75..29,15. Si le vrai ensemble sort de la plage ou ne permet pas un contact nu, **arrêter le montage de ce module** pour adapter la pièce ; le kit imprimé et ses variantes restent utilisables, mais aucune mise sous tension avec un contact douteux.

## 4. Câbles, accès et fermeture

La nappe sort côté ST-LINK avec une courbe douce, décalée vers X≈55,5, sans passer sur l’étrier avant. Largeur de calcul8mm pour nappe12conducteurs7,62mm ; les volumes IDC/câble incluent une marge de conception. Poser le Colson dans les fentes existantes, avec garniture ; ne pas utiliser l’IDC comme arrêt d’arrachement. Aucune valeur de rayon de courbure fabricant non publiée n’est supposée garantie.

Conserver les cassettes/trappes V2.5, la bride nappe rapprochée2,5mm et les17 M3×12+écrousDIN934 par coffret. Vérifier physiquementCN15, ST-LINK, la carteSD, le bornierRS485 et son outil, USER accessible et RESET distinct, les troisLED depuis la fenêtre et la nappe protégée. Les plans hérités sont inchangés ; la visibilité réelle et le passage des câbles dépendent du montage reçu.

Contrôle final à froid : pas de copeaux ou écrous libres ; têteM2/enveloppeC05 et composants dégagés ; pas de flexion dePCB ; fixations retenues ; ports libres ; aucun pont métallique de l’isolationClick ; couvercle libre sans pousser les réglages. Pour mémoire la marge calculée minimale des nouvelles têtesM2 au-dessus du STEP ST est0,29mm dans l’enveloppeG/rondelle retenue : vérifier la vraie hauteur de tête et l’empilage, ne pas ajouter de rondelle métallique non prévue.

La mise en service et la tenue au réducteur restent des étapes ultérieures. Le dossier d’impression ne constitue pas leur qualification.
